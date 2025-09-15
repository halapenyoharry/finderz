/* finderz-ds-store.h
 * 
 * .DS_Store file parser for Finderz
 * Reads macOS Finder metadata from network volumes and shared drives
 */

#ifndef FINDERZ_DS_STORE_H
#define FINDERZ_DS_STORE_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _FinderzDSStore FinderzDSStore;
typedef struct _FinderzDSStoreClass FinderzDSStoreClass;

/* DS_Store record types we care about */
typedef enum {
    FINDERZ_DS_STORE_ICON_LOCATION,
    FINDERZ_DS_STORE_WINDOW_BOUNDS,
    FINDERZ_DS_STORE_VIEW_STYLE,
    FINDERZ_DS_STORE_SORT_COLUMN,
    FINDERZ_DS_STORE_BACKGROUND_IMAGE,
    FINDERZ_DS_STORE_ICON_SIZE,
    FINDERZ_DS_STORE_TEXT_SIZE,
    FINDERZ_DS_STORE_LABEL_ON_BOTTOM,
    FINDERZ_DS_STORE_SHOW_ICON_PREVIEW,
    FINDERZ_DS_STORE_SHOW_ITEM_INFO
} FinderzDSStoreRecordType;

/* View styles from macOS Finder */
typedef enum {
    FINDERZ_VIEW_ICON = 0,
    FINDERZ_VIEW_LIST = 1,
    FINDERZ_VIEW_COLUMN = 2,
    FINDERZ_VIEW_GALLERY = 3
} FinderzViewStyle;

/* Icon position data */
typedef struct {
    gint x;
    gint y;
} FinderzIconPosition;

/* Structure to hold parsed DS_Store data for a directory */
typedef struct {
    FinderzViewStyle view_style;
    gint icon_size;
    gint text_size;
    gboolean label_on_bottom;
    gboolean show_icon_preview;
    gboolean show_item_info;
    gchar *sort_column;
    gchar *background_image_path;
    GHashTable *icon_locations; /* filename -> FinderzIconPosition* */
    GHashTable *color_labels; /* filename -> color label index */
    GdkRectangle window_bounds;
} FinderzDSStoreData;

/* Public functions */
FinderzDSStore* finderz_ds_store_new (void);
FinderzDSStoreData* finderz_ds_store_parse_file (FinderzDSStore *ds_store,
                                                  const gchar *ds_store_path,
                                                  GError **error);
void finderz_ds_store_data_free (FinderzDSStoreData *data);
gboolean finderz_ds_store_exists_for_directory (const gchar *directory_path);
FinderzDSStoreData* finderz_ds_store_get_cached_data (FinderzDSStore *ds_store,
                                                       const gchar *directory_path);

/* Get metadata for specific files */
FinderzIconPosition* finderz_ds_store_get_icon_position (FinderzDSStoreData *data,
                                                          const gchar *filename);
gint finderz_ds_store_get_color_label (FinderzDSStoreData *data,
                                        const gchar *filename);
const gchar* finderz_ds_store_get_sort_column (FinderzDSStoreData *data);

G_END_DECLS

#endif /* FINDERZ_DS_STORE_H */