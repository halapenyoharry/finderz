/* finderz-metadata-columns.c
 * 
 * Dynamic column provider for all metadata types
 * Creates columns on-the-fly based on discovered metadata
 */

#include <libnemo-extension/nemo-column.h>
#include <libnemo-extension/nemo-column-provider.h>
#include <glib/gi18n.h>
#include "finderz-universal-metadata.h"

typedef struct {
    GObject parent_instance;
    GHashTable *discovered_fields; /* Track all metadata fields we've seen */
} FinderzMetadataColumnProvider;

typedef struct {
    GObjectClass parent_class;
} FinderzMetadataColumnProviderClass;

static void finderz_metadata_column_provider_iface_init (NemoColumnProviderIface *iface);

G_DEFINE_TYPE_WITH_CODE (FinderzMetadataColumnProvider, 
                          finderz_metadata_column_provider, 
                          G_TYPE_OBJECT,
                          G_IMPLEMENT_INTERFACE (NEMO_TYPE_COLUMN_PROVIDER,
                                                finderz_metadata_column_provider_iface_init))

/* Standard metadata columns that are always available */
static GList *
get_standard_metadata_columns (void)
{
    GList *columns = NULL;
    
    /* === EXIF/Image Metadata === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_camera",
                                           "attribute", "exif_camera",
                                           "label", _("Camera"),
                                           "description", _("Camera make and model"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_lens",
                                           "attribute", "exif_lens",
                                           "label", _("Lens"),
                                           "description", _("Camera lens information"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_focal_length",
                                           "attribute", "exif_focal_length",
                                           "label", _("Focal Length"),
                                           "description", _("Focal length in mm"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_aperture",
                                           "attribute", "exif_aperture",
                                           "label", _("Aperture"),
                                           "description", _("F-stop value"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_iso",
                                           "attribute", "exif_iso",
                                           "label", _("ISO"),
                                           "description", _("ISO sensitivity"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "exif_date_taken",
                                           "attribute", "exif_date_taken",
                                           "label", _("Date Taken"),
                                           "description", _("Original photo date"),
                                           NULL));
    
    /* === AI Generation Metadata === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_prompt",
                                           "attribute", "ai_prompt",
                                           "label", _("AI Prompt"),
                                           "description", _("AI generation prompt"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_model",
                                           "attribute", "ai_model",
                                           "label", _("AI Model"),
                                           "description", _("AI model used"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_sampler",
                                           "attribute", "ai_sampler",
                                           "label", _("Sampler"),
                                           "description", _("AI sampling method"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_steps",
                                           "attribute", "ai_steps",
                                           "label", _("Steps"),
                                           "description", _("AI generation steps"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_cfg",
                                           "attribute", "ai_cfg",
                                           "label", _("CFG Scale"),
                                           "description", _("AI guidance scale"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_seed",
                                           "attribute", "ai_seed",
                                           "label", _("Seed"),
                                           "description", _("Random seed value"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "ai_tool",
                                           "attribute", "ai_tool",
                                           "label", _("AI Tool"),
                                           "description", _("Generation tool (ComfyUI, SD, etc)"),
                                           NULL));
    
    /* === Ratings and Labels === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "rating",
                                           "attribute", "rating",
                                           "label", _("Rating"),
                                           "description", _("Star rating (0-5)"),
                                           "xalign", 0.5,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "color_label",
                                           "attribute", "color_label",
                                           "label", _("Label"),
                                           "description", _("Color label"),
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "keywords",
                                           "attribute", "keywords",
                                           "label", _("Keywords"),
                                           "description", _("Keywords/tags"),
                                           NULL));
    
    /* === Location === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "gps_location",
                                           "attribute", "gps_location",
                                           "label", _("GPS Location"),
                                           "description", _("Latitude, Longitude"),
                                           NULL));
    
    /* === XMP/Sidecar === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "has_xmp",
                                           "attribute", "has_xmp",
                                           "label", _("XMP"),
                                           "description", _("Has XMP sidecar"),
                                           "xalign", 0.5,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "has_sidecar",
                                           "attribute", "has_sidecar",
                                           "label", _("Sidecar"),
                                           "description", _("Has metadata sidecar"),
                                           "xalign", 0.5,
                                           NULL));
    
    /* === Extended Attributes === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "xattr_count",
                                           "attribute", "xattr_count",
                                           "label", _("XAttrs"),
                                           "description", _("Extended attribute count"),
                                           "xalign", 1.0,
                                           NULL));
    
    /* === Dimensions (for images/videos) === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "dimensions",
                                           "attribute", "dimensions",
                                           "label", _("Dimensions"),
                                           "description", _("Width x Height"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "aspect_ratio",
                                           "attribute", "aspect_ratio",
                                           "label", _("Aspect"),
                                           "description", _("Aspect ratio"),
                                           "xalign", 1.0,
                                           NULL));
    
    /* === Audio/Video === */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "duration",
                                           "attribute", "duration",
                                           "label", _("Duration"),
                                           "description", _("Media duration"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "bitrate",
                                           "attribute", "bitrate",
                                           "label", _("Bitrate"),
                                           "description", _("Audio/video bitrate"),
                                           "xalign", 1.0,
                                           NULL));
    
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "codec",
                                           "attribute", "codec",
                                           "label", _("Codec"),
                                           "description", _("Audio/video codec"),
                                           NULL));
    
    return columns;
}

static GList *
finderz_metadata_column_provider_get_columns (NemoColumnProvider *provider)
{
    GList *columns = NULL;
    
    /* Get standard columns */
    columns = get_standard_metadata_columns ();
    
    /* TODO: Add dynamically discovered columns from 
     * FinderzMetadataColumnProvider->discovered_fields */
    
    return columns;
}

static void
finderz_metadata_column_provider_iface_init (NemoColumnProviderIface *iface)
{
    iface->get_columns = finderz_metadata_column_provider_get_columns;
}

static void
finderz_metadata_column_provider_init (FinderzMetadataColumnProvider *provider)
{
    provider->discovered_fields = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                          g_free, NULL);
}

static void
finderz_metadata_column_provider_finalize (GObject *object)
{
    FinderzMetadataColumnProvider *provider = (FinderzMetadataColumnProvider *)object;
    
    if (provider->discovered_fields) {
        g_hash_table_destroy (provider->discovered_fields);
    }
    
    G_OBJECT_CLASS (finderz_metadata_column_provider_parent_class)->finalize (object);
}

static void
finderz_metadata_column_provider_class_init (FinderzMetadataColumnProviderClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = finderz_metadata_column_provider_finalize;
}

/* Helper function to format metadata values for display */
gchar *
finderz_format_metadata_for_column (const gchar *attribute,
                                     FinderzUniversalMetadata *metadata)
{
    if (!metadata || !attribute) {
        return g_strdup ("");
    }
    
    FinderzMetadataField *field = finderz_get_metadata_field (metadata, attribute);
    if (!field) {
        return g_strdup ("");
    }
    
    /* Special formatting for certain types */
    if (g_strcmp0 (attribute, "rating") == 0) {
        gint rating = (gint)field->numeric_value;
        if (rating > 0) {
            GString *stars = g_string_new ("");
            for (int i = 0; i < rating; i++) {
                g_string_append (stars, "★");
            }
            for (int i = rating; i < 5; i++) {
                g_string_append (stars, "☆");
            }
            return g_string_free (stars, FALSE);
        }
    }
    else if (g_strcmp0 (attribute, "has_xmp") == 0 || 
             g_strcmp0 (attribute, "has_sidecar") == 0) {
        return g_strdup (field->value ? "✓" : "");
    }
    else if (field->date_value) {
        return g_date_time_format (field->date_value, "%Y-%m-%d %H:%M");
    }
    else if (field->numeric && field->numeric_value != 0) {
        return g_strdup_printf ("%.2f", field->numeric_value);
    }
    
    return g_strdup (field->value ? field->value : "");
}