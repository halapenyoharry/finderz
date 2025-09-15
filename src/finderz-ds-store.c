/* finderz-ds-store.c
 * 
 * Implementation of .DS_Store file parser for Finderz
 */

#include "finderz-ds-store.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

/* DS_Store file structure constants */
#define DS_STORE_MAGIC_1 0x00000001
#define DS_STORE_MAGIC_2 "Bud1"
#define DS_STORE_BLOCK_SIZE 4096

/* Record type codes found in DS_Store files */
#define DS_STORE_CODE_ILOC "Iloc"  /* Icon location */
#define DS_STORE_CODE_BWSP "bwsp"  /* Browser window settings plist */
#define DS_STORE_CODE_LSVP "lsvp"  /* List view settings plist */
#define DS_STORE_CODE_LSVP_CAP "lsvP"  /* List view settings plist (alternate) */
#define DS_STORE_CODE_ICVP "icvp"  /* Icon view settings plist */
#define DS_STORE_CODE_VMOD "vmod"  /* View mode */

struct _FinderzDSStore {
    GObject parent_instance;
    GHashTable *cache; /* directory_path -> FinderzDSStoreData */
};

struct _FinderzDSStoreClass {
    GObjectClass parent_class;
};

G_DEFINE_TYPE (FinderzDSStore, finderz_ds_store, G_TYPE_OBJECT)

static void
finderz_ds_store_finalize (GObject *object)
{
    FinderzDSStore *self = (FinderzDSStore *)object;
    
    if (self->cache) {
        g_hash_table_destroy (self->cache);
    }
    
    G_OBJECT_CLASS (finderz_ds_store_parent_class)->finalize (object);
}

static void
finderz_ds_store_class_init (FinderzDSStoreClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = finderz_ds_store_finalize;
}

static void
finderz_ds_store_init (FinderzDSStore *self)
{
    self->cache = g_hash_table_new_full (g_str_hash, g_str_equal,
                                          g_free,
                                          (GDestroyNotify)finderz_ds_store_data_free);
}

FinderzDSStore*
finderz_ds_store_new (void)
{
    return g_object_new (G_TYPE_OBJECT, NULL);
}

void
finderz_ds_store_data_free (FinderzDSStoreData *data)
{
    if (!data) return;
    
    g_free (data->sort_column);
    g_free (data->background_image_path);
    
    if (data->icon_locations) {
        g_hash_table_destroy (data->icon_locations);
    }
    
    if (data->color_labels) {
        g_hash_table_destroy (data->color_labels);
    }
    
    g_free (data);
}

static gboolean
read_ds_store_header (FILE *file, GError **error)
{
    guint32 magic1;
    char magic2[4];
    guint32 offset_info[3];
    
    /* Read first magic number (should be 0x00000001) */
    if (fread (&magic1, sizeof(guint32), 1, file) != 1) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to read DS_Store magic number 1");
        return FALSE;
    }
    
    /* Convert from big-endian */
    magic1 = ntohl(magic1);
    if (magic1 != DS_STORE_MAGIC_1) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Invalid DS_Store magic number 1: 0x%08x", magic1);
        return FALSE;
    }
    
    /* Read "Bud1" magic string */
    if (fread (magic2, 1, 4, file) != 4) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to read DS_Store magic number 2");
        return FALSE;
    }
    
    if (memcmp (magic2, DS_STORE_MAGIC_2, 4) != 0) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Invalid DS_Store file format - expected 'Bud1'");
        return FALSE;
    }
    
    /* Read offset information (we don't use it yet, but validates file) */
    if (fread (offset_info, sizeof(guint32), 3, file) != 3) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to read DS_Store offset information");
        return FALSE;
    }
    
    g_debug ("FINDERZ: Valid DS_Store header found - magic1=0x%08x, magic2='%c%c%c%c'",
             magic1, magic2[0], magic2[1], magic2[2], magic2[3]);
    
    return TRUE;
}

FinderzDSStoreData*
finderz_ds_store_parse_file (FinderzDSStore *ds_store,
                              const gchar *ds_store_path,
                              GError **error)
{
    FinderzDSStoreData *data;
    FILE *file;
    
    g_return_val_if_fail (ds_store != NULL, NULL);
    g_return_val_if_fail (ds_store_path != NULL, NULL);
    
    file = fopen (ds_store_path, "rb");
    if (!file) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                     "Could not open DS_Store file: %s", ds_store_path);
        return NULL;
    }
    
    if (!read_ds_store_header (file, error)) {
        fclose (file);
        return NULL;
    }
    
    data = g_new0 (FinderzDSStoreData, 1);
    
    /* Set defaults matching macOS Finder */
    data->view_style = FINDERZ_VIEW_ICON;
    data->icon_size = 64;
    data->text_size = 12;
    data->label_on_bottom = TRUE;
    data->show_icon_preview = TRUE;
    data->show_item_info = FALSE;
    data->sort_column = g_strdup ("name");
    data->icon_locations = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, g_free);
    data->color_labels = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                 g_free, g_free);
    
    /* TODO: Implement actual DS_Store parsing
     * This requires:
     * 1. Reading the B-tree structure
     * 2. Parsing record blocks
     * 3. Extracting relevant metadata
     * 
     * For now, return defaults that can be used for testing
     */
    
    fclose (file);
    
    /* Cache the parsed data */
    gchar *dir_path = g_path_get_dirname (ds_store_path);
    g_hash_table_insert (ds_store->cache, dir_path, data);
    
    return data;
}

gboolean
finderz_ds_store_exists_for_directory (const gchar *directory_path)
{
    gchar *ds_store_path;
    gboolean exists;
    
    g_return_val_if_fail (directory_path != NULL, FALSE);
    
    ds_store_path = g_build_filename (directory_path, ".DS_Store", NULL);
    exists = g_file_test (ds_store_path, G_FILE_TEST_EXISTS);
    g_free (ds_store_path);
    
    return exists;
}

FinderzDSStoreData*
finderz_ds_store_get_cached_data (FinderzDSStore *ds_store,
                                   const gchar *directory_path)
{
    g_return_val_if_fail (ds_store != NULL, NULL);
    g_return_val_if_fail (directory_path != NULL, NULL);
    
    return g_hash_table_lookup (ds_store->cache, directory_path);
}

/* Get icon position for a specific file */
FinderzIconPosition*
finderz_ds_store_get_icon_position (FinderzDSStoreData *data,
                                     const gchar *filename)
{
    if (!data || !data->icon_locations || !filename) {
        return NULL;
    }
    
    return g_hash_table_lookup (data->icon_locations, filename);
}

/* Get color label for a specific file */
gint
finderz_ds_store_get_color_label (FinderzDSStoreData *data,
                                   const gchar *filename)
{
    gpointer value;
    
    if (!data || !data->color_labels || !filename) {
        return -1;
    }
    
    value = g_hash_table_lookup (data->color_labels, filename);
    if (value) {
        return GPOINTER_TO_INT (value);
    }
    
    return -1;
}

/* Get the sort column preference */
const gchar*
finderz_ds_store_get_sort_column (FinderzDSStoreData *data)
{
    if (!data) {
        return NULL;
    }
    
    return data->sort_column;
}