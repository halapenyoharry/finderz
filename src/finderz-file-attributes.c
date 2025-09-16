/* finderz-file-attributes.c
 * 
 * Hook Finderz metadata into Nemo's attribute system
 * This makes our metadata appear in columns and be sortable
 */

#include <glib.h>
#include <libnemo-private/nemo-file.h>
#include "finderz-universal-metadata.h"
#include "finderz-xattr-handler.h"
#include "finderz-integration.h"

/* Cache of metadata for active files */
static GHashTable *metadata_cache = NULL;
static GMutex metadata_cache_mutex;

/* Initialize the metadata cache */
void
finderz_file_attributes_init (void)
{
    if (!metadata_cache) {
        metadata_cache = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                 g_free,
                                                 (GDestroyNotify)finderz_universal_metadata_free);
        g_mutex_init (&metadata_cache_mutex);
        
        /* Initialize metadata system */
        finderz_universal_metadata_init ();
    }
}

/* Get or load metadata for a file */
static FinderzUniversalMetadata*
get_or_load_metadata (const gchar *uri)
{
    FinderzUniversalMetadata *metadata;
    gchar *path;
    GFile *gfile;
    
    g_mutex_lock (&metadata_cache_mutex);
    
    /* Check cache first */
    metadata = g_hash_table_lookup (metadata_cache, uri);
    if (metadata) {
        g_mutex_unlock (&metadata_cache_mutex);
        return metadata;
    }
    
    /* Convert URI to path */
    gfile = g_file_new_for_uri (uri);
    path = g_file_get_path (gfile);
    g_object_unref (gfile);
    
    if (!path) {
        g_mutex_unlock (&metadata_cache_mutex);
        return NULL;
    }
    
    /* Extract metadata */
    GError *error = NULL;
    metadata = finderz_extract_all_metadata (path, &error);
    
    if (error) {
        g_debug ("FINDERZ: Failed to extract metadata for %s: %s", 
                 path, error->message);
        g_error_free (error);
    }
    
    /* Cache it */
    if (metadata) {
        g_hash_table_insert (metadata_cache, g_strdup (uri), metadata);
    }
    
    g_free (path);
    g_mutex_unlock (&metadata_cache_mutex);
    
    return metadata;
}

/* Get string value for a Finderz attribute */
gchar*
finderz_file_get_metadata_attribute (NemoFile *file, const gchar *attribute)
{
    gchar *uri;
    FinderzUniversalMetadata *metadata;
    FinderzMetadataField *field;
    gchar *result = NULL;
    
    if (!file || !attribute) {
        return NULL;
    }
    
    /* Get file URI */
    uri = nemo_file_get_uri (file);
    if (!uri) {
        return NULL;
    }
    
    /* Special handling for quick attributes */
    if (g_strcmp0 (attribute, "rating") == 0) {
        /* Try xattr first for speed */
        GFile *gfile = nemo_file_get_location (file);
        gchar *path = g_file_get_path (gfile);
        if (path) {
            gint rating = finderz_xattr_get_rating (path);
            if (rating > 0) {
                GString *stars = g_string_new ("");
                for (int i = 0; i < rating; i++) {
                    g_string_append (stars, "★");
                }
                for (int i = rating; i < 5; i++) {
                    g_string_append (stars, "☆");
                }
                result = g_string_free (stars, FALSE);
            }
            g_free (path);
        }
        g_object_unref (gfile);
        
        if (result) {
            g_free (uri);
            return result;
        }
    }
    
    /* Load full metadata */
    metadata = get_or_load_metadata (uri);
    if (!metadata) {
        g_free (uri);
        return NULL;
    }
    
    /* Get the field */
    field = finderz_get_metadata_field (metadata, attribute);
    if (field) {
        result = finderz_format_metadata_value (field);
    }
    
    g_free (uri);
    return result;
}

/* Check if we handle this attribute */
gboolean
finderz_is_metadata_attribute (const gchar *attribute)
{
    if (!attribute) {
        return FALSE;
    }
    
    /* Our metadata attributes */
    return (g_str_has_prefix (attribute, "ai_") ||
            g_str_has_prefix (attribute, "exif_") ||
            g_str_has_prefix (attribute, "mac_") ||
            g_str_has_prefix (attribute, "gps_") ||
            g_strcmp0 (attribute, "rating") == 0 ||
            g_strcmp0 (attribute, "color_label") == 0 ||
            g_strcmp0 (attribute, "keywords") == 0 ||
            g_strcmp0 (attribute, "has_xmp") == 0 ||
            g_strcmp0 (attribute, "has_sidecar") == 0 ||
            g_strcmp0 (attribute, "dimensions") == 0 ||
            g_strcmp0 (attribute, "aspect_ratio") == 0 ||
            g_strcmp0 (attribute, "duration") == 0 ||
            g_strcmp0 (attribute, "bitrate") == 0 ||
            g_strcmp0 (attribute, "codec") == 0);
}

/* Clear metadata cache for a file */
void
finderz_file_attributes_invalidate (const gchar *uri)
{
    g_mutex_lock (&metadata_cache_mutex);
    g_hash_table_remove (metadata_cache, uri);
    g_mutex_unlock (&metadata_cache_mutex);
}

/* Clear all cached metadata */
void
finderz_file_attributes_clear_cache (void)
{
    g_mutex_lock (&metadata_cache_mutex);
    g_hash_table_remove_all (metadata_cache);
    g_mutex_unlock (&metadata_cache_mutex);
}

/* Get sortable value for an attribute */
gint
finderz_file_compare_by_metadata (NemoFile *file1, NemoFile *file2, 
                                   const gchar *attribute)
{
    gchar *value1 = finderz_file_get_metadata_attribute (file1, attribute);
    gchar *value2 = finderz_file_get_metadata_attribute (file2, attribute);
    gint result;
    
    /* Handle NULL values */
    if (!value1 && !value2) {
        return 0;
    } else if (!value1) {
        g_free (value2);
        return -1;
    } else if (!value2) {
        g_free (value1);
        return 1;
    }
    
    /* Special handling for numeric fields */
    if (g_strcmp0 (attribute, "rating") == 0 ||
        g_strcmp0 (attribute, "ai_steps") == 0 ||
        g_strcmp0 (attribute, "ai_seed") == 0 ||
        g_strcmp0 (attribute, "exif_iso") == 0) {
        /* Compare as numbers */
        gint num1 = atoi (value1);
        gint num2 = atoi (value2);
        result = num1 - num2;
    } else {
        /* Compare as strings */
        result = g_utf8_collate (value1, value2);
    }
    
    g_free (value1);
    g_free (value2);
    
    return result;
}