/* finderz-sorting-profiles.h
 * 
 * Advanced sorting profiles system for Finderz
 * Allows multiple customizable sorting configurations
 */

#ifndef FINDERZ_SORTING_PROFILES_H
#define FINDERZ_SORTING_PROFILES_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _FinderzSortingProfile FinderzSortingProfile;
typedef struct _FinderzSortingProfileManager FinderzSortingProfileManager;

/* Sort criteria */
typedef enum {
    FINDERZ_SORT_BY_NAME,
    FINDERZ_SORT_BY_SIZE,
    FINDERZ_SORT_BY_TYPE,
    FINDERZ_SORT_BY_DATE_MODIFIED,
    FINDERZ_SORT_BY_DATE_CREATED,
    FINDERZ_SORT_BY_DATE_ACCESSED,
    FINDERZ_SORT_BY_EXTENSION,
    FINDERZ_SORT_BY_COLOR_LABEL,
    FINDERZ_SORT_BY_TAGS
} FinderzSortCriteria;

/* Sort direction */
typedef enum {
    FINDERZ_SORT_ASCENDING,
    FINDERZ_SORT_DESCENDING
} FinderzSortDirection;

/* Grouping options */
typedef enum {
    FINDERZ_GROUP_NONE,
    FINDERZ_GROUP_BY_KIND,
    FINDERZ_GROUP_BY_DATE_RANGE,
    FINDERZ_GROUP_BY_SIZE_RANGE,
    FINDERZ_GROUP_BY_FIRST_LETTER,
    FINDERZ_GROUP_BY_EXTENSION,
    FINDERZ_GROUP_BY_TAGS,
    FINDERZ_GROUP_BY_COLOR_LABEL,
    FINDERZ_GROUP_CUSTOM
} FinderzGroupingType;

/* Date range grouping */
typedef enum {
    FINDERZ_DATE_TODAY,
    FINDERZ_DATE_YESTERDAY,
    FINDERZ_DATE_THIS_WEEK,
    FINDERZ_DATE_LAST_WEEK,
    FINDERZ_DATE_THIS_MONTH,
    FINDERZ_DATE_LAST_MONTH,
    FINDERZ_DATE_THIS_YEAR,
    FINDERZ_DATE_OLDER,
    FINDERZ_DATE_CUSTOM_RANGE
} FinderzDateRange;

/* Size range grouping */
typedef struct {
    gchar *name;
    gint64 min_size;
    gint64 max_size;
} FinderzSizeRange;

/* Single sort level */
typedef struct {
    FinderzSortCriteria criteria;
    FinderzSortDirection direction;
} FinderzSortLevel;

/* Complete sorting profile */
struct _FinderzSortingProfile {
    gchar *name;
    gchar *description;
    gboolean is_default;
    
    /* Multi-level sorting (primary, secondary, tertiary) */
    GList *sort_levels; /* List of FinderzSortLevel */
    
    /* Grouping configuration */
    FinderzGroupingType grouping_type;
    gboolean show_groups_expanded;
    
    /* Custom grouping ranges */
    GList *custom_size_ranges; /* List of FinderzSizeRange */
    GList *custom_date_ranges; /* Custom date range definitions */
    
    /* Additional options */
    gboolean folders_first;
    gboolean show_hidden_files;
    gboolean case_sensitive;
    gboolean natural_sorting; /* e.g., file1, file2, file10 instead of file1, file10, file2 */
};

/* Profile manager */
struct _FinderzSortingProfileManager {
    GObject parent_instance;
    GList *profiles;
    FinderzSortingProfile *active_profile;
    GHashTable *directory_profiles; /* directory_path -> profile_name */
};

/* Public functions */
FinderzSortingProfileManager* finderz_sorting_profile_manager_new (void);
void finderz_sorting_profile_manager_free (FinderzSortingProfileManager *manager);

/* Profile management */
FinderzSortingProfile* finderz_sorting_profile_new (const gchar *name);
void finderz_sorting_profile_free (FinderzSortingProfile *profile);
FinderzSortingProfile* finderz_sorting_profile_copy (FinderzSortingProfile *profile);

/* Add/remove profiles */
void finderz_sorting_profile_manager_add_profile (FinderzSortingProfileManager *manager,
                                                   FinderzSortingProfile *profile);
void finderz_sorting_profile_manager_remove_profile (FinderzSortingProfileManager *manager,
                                                      const gchar *profile_name);
FinderzSortingProfile* finderz_sorting_profile_manager_get_profile (FinderzSortingProfileManager *manager,
                                                                     const gchar *profile_name);

/* Profile activation */
void finderz_sorting_profile_manager_set_active_profile (FinderzSortingProfileManager *manager,
                                                          const gchar *profile_name);
FinderzSortingProfile* finderz_sorting_profile_manager_get_active_profile (FinderzSortingProfileManager *manager);

/* Directory-specific profiles */
void finderz_sorting_profile_manager_set_directory_profile (FinderzSortingProfileManager *manager,
                                                             const gchar *directory_path,
                                                             const gchar *profile_name);
FinderzSortingProfile* finderz_sorting_profile_manager_get_directory_profile (FinderzSortingProfileManager *manager,
                                                                               const gchar *directory_path);

/* Profile persistence */
gboolean finderz_sorting_profile_manager_save_profiles (FinderzSortingProfileManager *manager,
                                                         const gchar *config_file,
                                                         GError **error);
gboolean finderz_sorting_profile_manager_load_profiles (FinderzSortingProfileManager *manager,
                                                         const gchar *config_file,
                                                         GError **error);

/* Built-in profiles */
void finderz_sorting_profile_manager_create_default_profiles (FinderzSortingProfileManager *manager);

/* Sorting functions */
gint finderz_sorting_profile_compare_files (FinderzSortingProfile *profile,
                                             GFileInfo *file1,
                                             GFileInfo *file2);

G_END_DECLS

#endif /* FINDERZ_SORTING_PROFILES_H */