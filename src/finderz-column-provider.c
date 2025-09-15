/* finderz-column-provider.c
 * 
 * Provides Mac metadata columns when DS_Store data is available
 */

#include <libnemo-extension/nemo-column.h>
#include <libnemo-extension/nemo-column-provider.h>
#include <glib/gi18n.h>
#include "finderz-integration.h"

typedef struct {
    GObject parent_instance;
} FinderzColumnProvider;

typedef struct {
    GObjectClass parent_class;
} FinderzColumnProviderClass;

static void finderz_column_provider_iface_init (NemoColumnProviderIface *iface);

G_DEFINE_TYPE_WITH_CODE (FinderzColumnProvider, finderz_column_provider, G_TYPE_OBJECT,
                          G_IMPLEMENT_INTERFACE (NEMO_TYPE_COLUMN_PROVIDER,
                                                finderz_column_provider_iface_init))

static GList *
finderz_column_provider_get_columns (NemoColumnProvider *provider)
{
    GList *columns = NULL;
    
    /* Mac icon position - only useful in icon view but shows DS_Store is being read */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "mac_icon_position",
                                           "attribute", "mac_icon_position",
                                           "label", _("Mac Position"),
                                           "description", _("Icon position from macOS Finder"),
                                           NULL));
    
    /* Mac view style preference */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "mac_view_style",
                                           "attribute", "mac_view_style",
                                           "label", _("Mac View"),
                                           "description", _("View style set in macOS Finder"),
                                           NULL));
    
    /* Mac color label */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "mac_color_label",
                                           "attribute", "mac_color_label",
                                           "label", _("Mac Label"),
                                           "description", _("Color label from macOS Finder"),
                                           NULL));
    
    /* Mac comment/spotlight comment */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "mac_comment",
                                           "attribute", "mac_comment",
                                           "label", _("Mac Comment"),
                                           "description", _("Spotlight comment from macOS"),
                                           NULL));
    
    /* DS_Store status - shows if folder has Mac metadata */
    columns = g_list_append (columns,
                             g_object_new (NEMO_TYPE_COLUMN,
                                           "name", "has_mac_metadata",
                                           "attribute", "has_mac_metadata",
                                           "label", _("Mac Metadata"),
                                           "description", _("Has .DS_Store file"),
                                           NULL));
    
    return columns;
}

static void
finderz_column_provider_iface_init (NemoColumnProviderIface *iface)
{
    iface->get_columns = finderz_column_provider_get_columns;
}

static void
finderz_column_provider_init (FinderzColumnProvider *provider)
{
}

static void
finderz_column_provider_class_init (FinderzColumnProviderClass *klass)
{
}

/* Register this provider */
void
finderz_column_provider_register (void)
{
    static GType type = 0;
    
    if (type == 0) {
        type = finderz_column_provider_get_type ();
    }
}

/* Get attribute value for a file */
gchar *
finderz_get_mac_attribute_for_file (const gchar *attribute,
                                     const gchar *file_path,
                                     const gchar *parent_dir)
{
    if (g_strcmp0 (attribute, "has_mac_metadata") == 0) {
        /* For directories, check if they contain DS_Store */
        if (g_file_test (file_path, G_FILE_TEST_IS_DIR)) {
            if (finderz_directory_has_mac_metadata (file_path)) {
                return g_strdup ("✓");
            }
        }
        /* For files in a directory with DS_Store, could show position */
        else if (parent_dir && finderz_directory_has_mac_metadata (parent_dir)) {
            return g_strdup ("•");
        }
        return g_strdup ("");
    }
    else if (g_strcmp0 (attribute, "mac_view_style") == 0) {
        if (g_file_test (file_path, G_FILE_TEST_IS_DIR)) {
            gint view_style = finderz_get_mac_view_style (file_path);
            switch (view_style) {
                case 0: return g_strdup ("Icon");
                case 1: return g_strdup ("List");
                case 2: return g_strdup ("Column");
                case 3: return g_strdup ("Gallery");
                default: return g_strdup ("");
            }
        }
        return g_strdup ("");
    }
    else if (g_strcmp0 (attribute, "mac_icon_position") == 0) {
        /* TODO: Get actual position from DS_Store for this file */
        if (parent_dir && finderz_directory_has_mac_metadata (parent_dir)) {
            return g_strdup ("(x,y)");
        }
        return g_strdup ("");
    }
    else if (g_strcmp0 (attribute, "mac_color_label") == 0) {
        /* TODO: Parse color label from DS_Store */
        return g_strdup ("");
    }
    else if (g_strcmp0 (attribute, "mac_comment") == 0) {
        /* TODO: Get Spotlight comment if available */
        return g_strdup ("");
    }
    
    return NULL;
}