/* finderz-file-attributes.h
 * 
 * Hook Finderz metadata into Nemo's attribute system
 */

#ifndef FINDERZ_FILE_ATTRIBUTES_H
#define FINDERZ_FILE_ATTRIBUTES_H

#include <glib.h>
#include <libnemo-private/nemo-file.h>

G_BEGIN_DECLS

/* Initialize the metadata attribute system */
void finderz_file_attributes_init (void);

/* Get string value for a Finderz attribute */
gchar* finderz_file_get_metadata_attribute (NemoFile *file, const gchar *attribute);

/* Check if we handle this attribute */
gboolean finderz_is_metadata_attribute (const gchar *attribute);

/* Clear metadata cache for a file */
void finderz_file_attributes_invalidate (const gchar *uri);

/* Clear all cached metadata */
void finderz_file_attributes_clear_cache (void);

/* Compare files by metadata attribute for sorting */
gint finderz_file_compare_by_metadata (NemoFile *file1, NemoFile *file2, 
                                        const gchar *attribute);

G_END_DECLS

#endif /* FINDERZ_FILE_ATTRIBUTES_H */