/* finderz-universal-metadata.c
 * 
 * Implementation of universal metadata system
 */

#include "finderz-universal-metadata.h"
#include <string.h>

/* Free a metadata field */
void
finderz_metadata_field_free (FinderzMetadataField *field)
{
    if (!field) return;
    
    g_free (field->key);
    g_free (field->value);
    g_free (field->display_name);
    g_free (field->category);
    if (field->date_value) {
        g_date_time_unref (field->date_value);
    }
    g_free (field);
}

/* Free AI metadata */
void
finderz_ai_metadata_free (FinderzAIMetadata *metadata)
{
    if (!metadata) return;
    
    g_free (metadata->prompt);
    g_free (metadata->negative_prompt);
    g_free (metadata->model);
    g_free (metadata->sampler);
    g_free (metadata->tool);
    g_free (metadata->workflow);
    
    if (metadata->loras) {
        g_hash_table_destroy (metadata->loras);
    }
    if (metadata->custom) {
        g_hash_table_destroy (metadata->custom);
    }
    
    g_free (metadata);
}

/* Free image metadata */
void
finderz_image_metadata_free (FinderzImageMetadata *metadata)
{
    if (!metadata) return;
    
    g_free (metadata->camera_make);
    g_free (metadata->camera_model);
    g_free (metadata->lens);
    if (metadata->date_taken) {
        g_date_time_unref (metadata->date_taken);
    }
    
    finderz_ai_metadata_free (metadata->ai_data);
    
    g_free (metadata->color_label);
    g_free (metadata->caption);
    g_list_free_full (metadata->keywords, g_free);
    
    g_free (metadata);
}

/* Free universal metadata */
void
finderz_universal_metadata_free (FinderzUniversalMetadata *metadata)
{
    if (!metadata) return;
    
    if (metadata->fields) {
        g_hash_table_destroy (metadata->fields);
    }
    g_list_free_full (metadata->categories, g_free);
    g_free (metadata->file_path);
    if (metadata->last_extracted) {
        g_date_time_unref (metadata->last_extracted);
    }
    
    g_free (metadata);
}

/* Get metadata field */
FinderzMetadataField*
finderz_get_metadata_field (FinderzUniversalMetadata *metadata,
                             const gchar *key)
{
    if (!metadata || !metadata->fields || !key) {
        return NULL;
    }
    
    return g_hash_table_lookup (metadata->fields, key);
}

/* Get all fields in a category */
GList*
finderz_get_metadata_by_category (FinderzUniversalMetadata *metadata,
                                   const gchar *category)
{
    GList *fields = NULL;
    
    if (!metadata || !metadata->fields || !category) {
        return NULL;
    }
    
    GHashTableIter iter;
    gpointer key, value;
    
    g_hash_table_iter_init (&iter, metadata->fields);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        FinderzMetadataField *field = value;
        if (field->category && g_strcmp0 (field->category, category) == 0) {
            fields = g_list_append (fields, field);
        }
    }
    
    return fields;
}

/* Format metadata for display */
gchar*
finderz_format_metadata_value (FinderzMetadataField *field)
{
    if (!field || !field->value) {
        return g_strdup ("");
    }
    
    /* Special formatting for certain types */
    if (g_strcmp0 (field->key, "rating") == 0 && field->numeric) {
        /* Show as stars */
        GString *stars = g_string_new ("");
        gint rating = (gint)field->numeric_value;
        for (int i = 0; i < rating; i++) {
            g_string_append (stars, "★");
        }
        for (int i = rating; i < 5; i++) {
            g_string_append (stars, "☆");
        }
        return g_string_free (stars, FALSE);
    }
    else if (field->date_value) {
        return g_date_time_format (field->date_value, "%Y-%m-%d %H:%M");
    }
    else if (field->numeric) {
        if (field->numeric_value == (gint)field->numeric_value) {
            return g_strdup_printf ("%d", (gint)field->numeric_value);
        } else {
            return g_strdup_printf ("%.2f", field->numeric_value);
        }
    }
    
    /* Truncate very long values for display */
    if (strlen (field->value) > 100) {
        gchar *truncated = g_strndup (field->value, 97);
        gchar *result = g_strdup_printf ("%s...", truncated);
        g_free (truncated);
        return result;
    }
    
    return g_strdup (field->value);
}

/* Check for sidecar files */
gchar*
finderz_find_xmp_sidecar (const gchar *file_path)
{
    gchar *xmp_path = g_strdup_printf ("%s.xmp", file_path);
    if (g_file_test (xmp_path, G_FILE_TEST_EXISTS)) {
        return xmp_path;
    }
    g_free (xmp_path);
    
    /* Try without extension */
    gchar *base = g_strndup (file_path, strrchr (file_path, '.') - file_path);
    xmp_path = g_strdup_printf ("%s.xmp", base);
    g_free (base);
    
    if (g_file_test (xmp_path, G_FILE_TEST_EXISTS)) {
        return xmp_path;
    }
    g_free (xmp_path);
    
    return NULL;
}

gchar*
finderz_find_metadata_sidecar (const gchar *file_path)
{
    /* Check for various sidecar formats */
    const gchar *extensions[] = {".metadata", ".json", ".meta", NULL};
    
    for (int i = 0; extensions[i]; i++) {
        gchar *sidecar_path = g_strdup_printf ("%s%s", file_path, extensions[i]);
        if (g_file_test (sidecar_path, G_FILE_TEST_EXISTS)) {
            return sidecar_path;
        }
        g_free (sidecar_path);
    }
    
    return NULL;
}

/* Get list of available metadata columns */
GList*
finderz_get_available_metadata_columns (void)
{
    GList *columns = NULL;
    
    /* Standard columns always available */
    columns = g_list_append (columns, g_strdup ("rating"));
    columns = g_list_append (columns, g_strdup ("ai_prompt"));
    columns = g_list_append (columns, g_strdup ("ai_model"));
    columns = g_list_append (columns, g_strdup ("ai_sampler"));
    columns = g_list_append (columns, g_strdup ("ai_steps"));
    columns = g_list_append (columns, g_strdup ("ai_cfg"));
    columns = g_list_append (columns, g_strdup ("ai_seed"));
    columns = g_list_append (columns, g_strdup ("exif_camera"));
    columns = g_list_append (columns, g_strdup ("exif_lens"));
    columns = g_list_append (columns, g_strdup ("exif_iso"));
    columns = g_list_append (columns, g_strdup ("exif_aperture"));
    columns = g_list_append (columns, g_strdup ("exif_focal_length"));
    columns = g_list_append (columns, g_strdup ("gps_location"));
    columns = g_list_append (columns, g_strdup ("keywords"));
    columns = g_list_append (columns, g_strdup ("color_label"));
    
    /* TODO: Dynamically discover more based on actual files */
    
    return columns;
}

/* Initialize metadata system */
void
finderz_universal_metadata_init (void)
{
    g_debug ("FINDERZ: Initializing universal metadata system");
    
    /* Initialize subsystems */
    extern void finderz_xattr_init (void);
    finderz_xattr_init ();
    
    /* TODO: Initialize libexiv2 */
    /* TODO: Initialize media extractors */
    
    g_debug ("FINDERZ: Metadata system initialized");
}