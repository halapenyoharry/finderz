/* finderz-exif-extractor.c
 * 
 * EXIF, IPTC, and XMP metadata extraction
 * Handles camera data, AI generation metadata, ratings, etc.
 */

#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include "finderz-universal-metadata.h"
#include "finderz-xattr-handler.h"

/* For now, we'll use basic extraction. Later integrate libexiv2 */

/* PNG text chunk structure */
typedef struct {
    guint32 length;
    gchar type[4];
    /* data follows */
} PNGChunk;

/* Extract metadata from PNG text chunks (ComfyUI, SD, etc) */
static GHashTable*
extract_png_metadata (const gchar *filepath)
{
    GHashTable *metadata = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, g_free);
    FILE *file = fopen (filepath, "rb");
    if (!file) return metadata;
    
    /* Check PNG signature */
    guchar signature[8];
    if (fread (signature, 1, 8, file) != 8) {
        fclose (file);
        return metadata;
    }
    
    /* PNG signature: 89 50 4E 47 0D 0A 1A 0A */
    if (signature[0] != 0x89 || signature[1] != 'P' || 
        signature[2] != 'N' || signature[3] != 'G') {
        fclose (file);
        return metadata;
    }
    
    /* Read chunks */
    while (!feof (file)) {
        guint32 length;
        gchar type[5] = {0};
        
        if (fread (&length, 4, 1, file) != 1) break;
        length = GUINT32_FROM_BE (length);
        
        if (fread (type, 1, 4, file) != 4) break;
        
        /* Look for text chunks */
        if (strcmp (type, "tEXt") == 0 || strcmp (type, "iTXt") == 0) {
            gchar *data = g_malloc (length + 1);
            if (fread (data, 1, length, file) == length) {
                data[length] = '\0';
                
                /* Parse key=value */
                gchar *value = strchr (data, '\0');
                if (value && value < data + length - 1) {
                    value++; /* Skip null terminator */
                    
                    /* Common AI metadata keys */
                    if (strcmp (data, "parameters") == 0 ||
                        strcmp (data, "prompt") == 0 ||
                        strcmp (data, "workflow") == 0 ||
                        strcmp (data, "Dream") == 0) {
                        
                        g_hash_table_insert (metadata, 
                                             g_strdup (data),
                                             g_strdup (value));
                        
                        /* Parse SD/ComfyUI parameters */
                        if (strstr (value, "Steps:") || strstr (value, "Sampler:")) {
                            /* Parse Stable Diffusion format */
                            parse_sd_parameters (metadata, value);
                        }
                    }
                }
            }
            g_free (data);
            
            /* Skip CRC */
            fseek (file, 4, SEEK_CUR);
        } else if (strcmp (type, "IEND") == 0) {
            break;
        } else {
            /* Skip chunk data and CRC */
            fseek (file, length + 4, SEEK_CUR);
        }
    }
    
    fclose (file);
    return metadata;
}

/* Parse Stable Diffusion parameter string */
static void
parse_sd_parameters (GHashTable *metadata, const gchar *params)
{
    /* Format: "prompt text\nNegative prompt: negative\nSteps: 20, Sampler: Euler a, CFG scale: 7" */
    
    gchar **lines = g_strsplit (params, "\n", -1);
    for (int i = 0; lines[i]; i++) {
        gchar *line = lines[i];
        
        if (g_str_has_prefix (line, "Negative prompt:")) {
            g_hash_table_insert (metadata,
                                 g_strdup ("negative_prompt"),
                                 g_strdup (line + 16));
        } else if (strstr (line, "Steps:")) {
            /* Parse the parameter line */
            gchar **parts = g_strsplit (line, ", ", -1);
            for (int j = 0; parts[j]; j++) {
                gchar **kv = g_strsplit (parts[j], ": ", 2);
                if (kv[0] && kv[1]) {
                    /* Normalize keys */
                    gchar *key = g_ascii_strdown (kv[0], -1);
                    g_strdelimit (key, " ", '_');
                    g_hash_table_insert (metadata, key, g_strdup (kv[1]));
                }
                g_strfreev (kv);
            }
            g_strfreev (parts);
        } else if (i == 0 && !strstr (line, ":")) {
            /* First line is usually the prompt */
            g_hash_table_insert (metadata,
                                 g_strdup ("prompt"),
                                 g_strdup (line));
        }
    }
    g_strfreev (lines);
}

/* Extract basic image metadata using GdkPixbuf (temporary solution) */
FinderzImageMetadata*
finderz_extract_image_metadata_basic (const gchar *filepath)
{
    FinderzImageMetadata *img_meta = g_new0 (FinderzImageMetadata, 1);
    
    /* Try to get from extended attributes first (cached) */
    img_meta->rating = finderz_xattr_get_rating (filepath);
    
    gchar *ai_prompt = finderz_xattr_get_ai_prompt (filepath);
    gchar *ai_model = finderz_xattr_get_ai_model (filepath);
    
    if (ai_prompt || ai_model) {
        img_meta->ai_data = g_new0 (FinderzAIMetadata, 1);
        img_meta->ai_data->prompt = ai_prompt;
        img_meta->ai_data->model = ai_model;
    }
    
    /* Check if PNG and extract embedded metadata */
    if (g_str_has_suffix (filepath, ".png") || 
        g_str_has_suffix (filepath, ".PNG")) {
        
        GHashTable *png_meta = extract_png_metadata (filepath);
        
        if (g_hash_table_size (png_meta) > 0) {
            if (!img_meta->ai_data) {
                img_meta->ai_data = g_new0 (FinderzAIMetadata, 1);
            }
            
            /* Extract AI fields */
            gchar *value;
            if ((value = g_hash_table_lookup (png_meta, "prompt"))) {
                g_free (img_meta->ai_data->prompt);
                img_meta->ai_data->prompt = g_strdup (value);
            }
            if ((value = g_hash_table_lookup (png_meta, "negative_prompt"))) {
                img_meta->ai_data->negative_prompt = g_strdup (value);
            }
            if ((value = g_hash_table_lookup (png_meta, "sampler"))) {
                img_meta->ai_data->sampler = g_strdup (value);
            }
            if ((value = g_hash_table_lookup (png_meta, "steps"))) {
                img_meta->ai_data->steps = atoi (value);
            }
            if ((value = g_hash_table_lookup (png_meta, "cfg_scale"))) {
                img_meta->ai_data->cfg_scale = g_ascii_strtod (value, NULL);
            }
            if ((value = g_hash_table_lookup (png_meta, "seed"))) {
                img_meta->ai_data->seed = atoi (value);
            }
            
            /* Detect tool */
            if (g_hash_table_lookup (png_meta, "workflow")) {
                img_meta->ai_data->tool = g_strdup ("ComfyUI");
            } else if (g_hash_table_lookup (png_meta, "parameters")) {
                img_meta->ai_data->tool = g_strdup ("Stable Diffusion WebUI");
            }
            
            /* Cache in xattrs for fast access */
            if (img_meta->ai_data->prompt) {
                finderz_xattr_set_ai_prompt (filepath, img_meta->ai_data->prompt);
            }
            if (img_meta->ai_data->model) {
                finderz_xattr_set_ai_model (filepath, img_meta->ai_data->model);
            }
        }
        
        g_hash_table_destroy (png_meta);
    }
    
    return img_meta;
}

/* Convert image metadata to universal metadata format */
FinderzUniversalMetadata*
finderz_image_to_universal_metadata (FinderzImageMetadata *img_meta,
                                      const gchar *filepath)
{
    FinderzUniversalMetadata *meta = g_new0 (FinderzUniversalMetadata, 1);
    meta->fields = g_hash_table_new_full (g_str_hash, g_str_equal,
                                           g_free, 
                                           (GDestroyNotify)finderz_metadata_field_free);
    meta->file_path = g_strdup (filepath);
    meta->last_extracted = g_date_time_new_now_local ();
    
    /* Add rating */
    if (img_meta->rating > 0) {
        FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
        field->key = g_strdup ("rating");
        field->value = g_strdup_printf ("%d", img_meta->rating);
        field->display_name = g_strdup ("Rating");
        field->category = g_strdup ("General");
        field->sortable = TRUE;
        field->numeric = TRUE;
        field->numeric_value = img_meta->rating;
        g_hash_table_insert (meta->fields, g_strdup ("rating"), field);
    }
    
    /* Add AI metadata */
    if (img_meta->ai_data) {
        FinderzAIMetadata *ai = img_meta->ai_data;
        
        if (ai->prompt) {
            FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
            field->key = g_strdup ("ai_prompt");
            field->value = g_strdup (ai->prompt);
            field->display_name = g_strdup ("AI Prompt");
            field->category = g_strdup ("AI Generation");
            field->sortable = TRUE;
            g_hash_table_insert (meta->fields, g_strdup ("ai_prompt"), field);
        }
        
        if (ai->model) {
            FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
            field->key = g_strdup ("ai_model");
            field->value = g_strdup (ai->model);
            field->display_name = g_strdup ("AI Model");
            field->category = g_strdup ("AI Generation");
            field->sortable = TRUE;
            g_hash_table_insert (meta->fields, g_strdup ("ai_model"), field);
        }
        
        if (ai->sampler) {
            FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
            field->key = g_strdup ("ai_sampler");
            field->value = g_strdup (ai->sampler);
            field->display_name = g_strdup ("Sampler");
            field->category = g_strdup ("AI Generation");
            field->sortable = TRUE;
            g_hash_table_insert (meta->fields, g_strdup ("ai_sampler"), field);
        }
        
        if (ai->steps > 0) {
            FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
            field->key = g_strdup ("ai_steps");
            field->value = g_strdup_printf ("%d", ai->steps);
            field->display_name = g_strdup ("Steps");
            field->category = g_strdup ("AI Generation");
            field->sortable = TRUE;
            field->numeric = TRUE;
            field->numeric_value = ai->steps;
            g_hash_table_insert (meta->fields, g_strdup ("ai_steps"), field);
        }
    }
    
    return meta;
}

/* Main extraction function */
FinderzUniversalMetadata*
finderz_extract_all_metadata (const gchar *filepath, GError **error)
{
    if (!g_file_test (filepath, G_FILE_TEST_EXISTS)) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                     "File not found: %s", filepath);
        return NULL;
    }
    
    /* Determine file type and extract accordingly */
    FinderzUniversalMetadata *metadata = NULL;
    
    if (g_str_has_suffix (filepath, ".png") || 
        g_str_has_suffix (filepath, ".PNG") ||
        g_str_has_suffix (filepath, ".jpg") ||
        g_str_has_suffix (filepath, ".jpeg") ||
        g_str_has_suffix (filepath, ".webp")) {
        
        FinderzImageMetadata *img_meta = finderz_extract_image_metadata_basic (filepath);
        metadata = finderz_image_to_universal_metadata (img_meta, filepath);
        finderz_image_metadata_free (img_meta);
    } else {
        /* Generic metadata container */
        metadata = g_new0 (FinderzUniversalMetadata, 1);
        metadata->fields = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free,
                                                   (GDestroyNotify)finderz_metadata_field_free);
        metadata->file_path = g_strdup (filepath);
        metadata->last_extracted = g_date_time_new_now_local ();
    }
    
    /* Always add xattr metadata */
    GHashTable *xattrs = finderz_xattr_get_all_finderz (filepath);
    if (xattrs) {
        GHashTableIter iter;
        gpointer key, value;
        
        g_hash_table_iter_init (&iter, xattrs);
        while (g_hash_table_iter_next (&iter, &key, &value)) {
            /* Don't duplicate if already extracted */
            if (!g_hash_table_lookup (metadata->fields, key)) {
                FinderzMetadataField *field = g_new0 (FinderzMetadataField, 1);
                field->key = g_strdup (key);
                field->value = g_strdup (value);
                field->display_name = g_strdup (key);
                field->category = g_strdup ("Extended Attributes");
                field->sortable = TRUE;
                field->source = FINDERZ_METADATA_SOURCE_XATTR;
                g_hash_table_insert (metadata->fields, g_strdup (key), field);
            }
        }
        g_hash_table_destroy (xattrs);
    }
    
    return metadata;
}