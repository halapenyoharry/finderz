/* finderz-universal-metadata.h
 * 
 * Universal metadata extraction and exposure system
 * Makes ALL metadata available for sorting/viewing/filtering
 */

#ifndef FINDERZ_UNIVERSAL_METADATA_H
#define FINDERZ_UNIVERSAL_METADATA_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* Metadata sources */
typedef enum {
    FINDERZ_METADATA_SOURCE_EXIF,           /* JPEG/TIFF EXIF data */
    FINDERZ_METADATA_SOURCE_XMP,            /* XMP sidecar files or embedded */
    FINDERZ_METADATA_SOURCE_IPTC,           /* IPTC photo metadata */
    FINDERZ_METADATA_SOURCE_DS_STORE,       /* macOS .DS_Store */
    FINDERZ_METADATA_SOURCE_XATTR,          /* Extended attributes */
    FINDERZ_METADATA_SOURCE_AI_WORKFLOW,    /* ComfyUI/SD workflows */
    FINDERZ_METADATA_SOURCE_PNG_TEXT,       /* PNG text chunks */
    FINDERZ_METADATA_SOURCE_WEBP_METADATA,  /* WebP metadata */
    FINDERZ_METADATA_SOURCE_ID3,            /* MP3/audio tags */
    FINDERZ_METADATA_SOURCE_VIDEO,          /* Video metadata */
    FINDERZ_METADATA_SOURCE_PDF,            /* PDF metadata */
    FINDERZ_METADATA_SOURCE_OFFICE,         /* Office document properties */
    FINDERZ_METADATA_SOURCE_SIDECAR         /* Any .metadata or similar sidecar */
} FinderzMetadataSource;

/* Metadata field - generic container for any metadata */
typedef struct {
    gchar *key;          /* Field name/key */
    gchar *value;        /* String value */
    gchar *display_name; /* Human-readable name */
    gchar *category;     /* Category for grouping */
    FinderzMetadataSource source;
    gboolean sortable;   /* Can this field be used for sorting? */
    gboolean numeric;    /* Is this a numeric field? */
    GDateTime *date_value; /* For date fields */
    gdouble numeric_value; /* For numeric fields */
} FinderzMetadataField;

/* Universal metadata container */
typedef struct {
    GHashTable *fields;  /* key -> FinderzMetadataField */
    GList *categories;   /* List of categories found */
    gchar *file_path;
    GDateTime *last_extracted;
} FinderzUniversalMetadata;

/* AI-specific metadata */
typedef struct {
    gchar *prompt;
    gchar *negative_prompt;
    gchar *model;
    gchar *sampler;
    gint steps;
    gdouble cfg_scale;
    gint seed;
    gchar *tool;        /* ComfyUI, SD WebUI, Midjourney, etc */
    gchar *workflow;    /* Full workflow JSON if available */
    GHashTable *loras;  /* LoRA models used */
    GHashTable *custom; /* Tool-specific fields */
} FinderzAIMetadata;

/* Image-specific metadata */
typedef struct {
    /* Basic EXIF */
    gchar *camera_make;
    gchar *camera_model;
    gchar *lens;
    gdouble focal_length;
    gdouble aperture;
    gdouble shutter_speed;
    gint iso;
    GDateTime *date_taken;
    
    /* Location */
    gdouble latitude;
    gdouble longitude;
    gdouble altitude;
    
    /* AI Generation */
    FinderzAIMetadata *ai_data;
    
    /* Ratings/Labels */
    gint rating;        /* 0-5 stars */
    gchar *color_label;
    GList *keywords;
    gchar *caption;
} FinderzImageMetadata;

/* Public functions */

/* Initialize metadata system */
void finderz_universal_metadata_init (void);

/* Extract all available metadata from a file */
FinderzUniversalMetadata* finderz_extract_all_metadata (const gchar *file_path,
                                                         GError **error);

/* Extract specific metadata type */
FinderzImageMetadata* finderz_extract_image_metadata (const gchar *file_path,
                                                       GError **error);
FinderzAIMetadata* finderz_extract_ai_metadata (const gchar *file_path,
                                                 GError **error);

/* Get metadata field for display/sorting */
FinderzMetadataField* finderz_get_metadata_field (FinderzUniversalMetadata *metadata,
                                                   const gchar *key);

/* Get all fields in a category */
GList* finderz_get_metadata_by_category (FinderzUniversalMetadata *metadata,
                                          const gchar *category);

/* Check for sidecar files */
gchar* finderz_find_xmp_sidecar (const gchar *file_path);
gchar* finderz_find_metadata_sidecar (const gchar *file_path);

/* Free functions */
void finderz_universal_metadata_free (FinderzUniversalMetadata *metadata);
void finderz_image_metadata_free (FinderzImageMetadata *metadata);
void finderz_ai_metadata_free (FinderzAIMetadata *metadata);
void finderz_metadata_field_free (FinderzMetadataField *field);

/* Get list of all possible metadata fields for column selection */
GList* finderz_get_available_metadata_columns (void);

/* Format metadata for display */
gchar* finderz_format_metadata_value (FinderzMetadataField *field);

/* Metadata writing (future) */
gboolean finderz_write_metadata_field (const gchar *file_path,
                                        const gchar *key,
                                        const gchar *value,
                                        GError **error);

G_END_DECLS

#endif /* FINDERZ_UNIVERSAL_METADATA_H */