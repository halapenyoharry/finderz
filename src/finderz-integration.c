/* finderz-integration.c
 * 
 * Integration points for Finderz features in Nemo
 */

#include <glib.h>
#include <gio/gio.h>
#include "finderz-ds-store.h"

static FinderzDSStore *global_ds_store_parser = NULL;

/* Initialize Finderz features */
void
finderz_init (void)
{
    g_debug ("FINDERZ: Initializing Finderz features...");
    
    if (!global_ds_store_parser) {
        global_ds_store_parser = finderz_ds_store_new ();
        g_debug ("FINDERZ: DS_Store parser initialized");
    }
}

/* Check if a directory has Mac metadata */
gboolean
finderz_directory_has_mac_metadata (const gchar *directory_path)
{
    gboolean has_ds_store;
    gchar *ds_store_path;
    
    if (!directory_path) {
        return FALSE;
    }
    
    ds_store_path = g_build_filename (directory_path, ".DS_Store", NULL);
    has_ds_store = g_file_test (ds_store_path, G_FILE_TEST_EXISTS);
    
    if (has_ds_store) {
        gint64 size = 0;
        GFile *file = g_file_new_for_path (ds_store_path);
        GFileInfo *info = g_file_query_info (file,
                                              G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                              G_FILE_QUERY_INFO_NONE,
                                              NULL, NULL);
        
        if (info) {
            size = g_file_info_get_size (info);
            g_object_unref (info);
        }
        
        g_debug ("FINDERZ: Found .DS_Store in %s (size: %ld bytes)", 
                 directory_path, (long)size);
        
        /* Try to parse it */
        if (global_ds_store_parser) {
            GError *error = NULL;
            FinderzDSStoreData *data = finderz_ds_store_parse_file (
                global_ds_store_parser, ds_store_path, &error);
            
            if (data) {
                g_debug ("FINDERZ: Successfully parsed DS_Store - view style: %d", 
                         data->view_style);
                finderz_ds_store_data_free (data);
            } else if (error) {
                g_debug ("FINDERZ: Failed to parse DS_Store: %s", error->message);
                g_error_free (error);
            }
        }
        
        g_object_unref (file);
    }
    
    g_free (ds_store_path);
    return has_ds_store;
}

/* Get view style from DS_Store if available */
gint
finderz_get_mac_view_style (const gchar *directory_path)
{
    if (!global_ds_store_parser || !directory_path) {
        return -1;
    }
    
    FinderzDSStoreData *cached = finderz_ds_store_get_cached_data (
        global_ds_store_parser, directory_path);
    
    if (cached) {
        return cached->view_style;
    }
    
    /* Try to load it */
    if (finderz_directory_has_mac_metadata (directory_path)) {
        cached = finderz_ds_store_get_cached_data (
            global_ds_store_parser, directory_path);
        if (cached) {
            return cached->view_style;
        }
    }
    
    return -1;
}

/* Cleanup Finderz features */
void
finderz_cleanup (void)
{
    if (global_ds_store_parser) {
        g_object_unref (global_ds_store_parser);
        global_ds_store_parser = NULL;
        g_debug ("FINDERZ: Cleaned up DS_Store parser");
    }
}