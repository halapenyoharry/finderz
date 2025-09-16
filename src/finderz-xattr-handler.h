/* finderz-xattr-handler.h
 * 
 * Extended attributes handler for indelible metadata storage
 */

#ifndef FINDERZ_XATTR_HANDLER_H
#define FINDERZ_XATTR_HANDLER_H

#include <glib.h>

G_BEGIN_DECLS

/* Check if filesystem supports extended attributes */
gboolean finderz_xattr_supported (const gchar *path);

/* List all extended attributes for a file */
GList* finderz_xattr_list (const gchar *path, GError **error);

/* Get value of an extended attribute */
gchar* finderz_xattr_get (const gchar *path, const gchar *name, GError **error);

/* Set value of an extended attribute */
gboolean finderz_xattr_set (const gchar *path, const gchar *name, 
                            const gchar *value, GError **error);

/* Remove an extended attribute */
gboolean finderz_xattr_remove (const gchar *path, const gchar *name, GError **error);

/* Get all Finderz-specific attributes as a hash table */
GHashTable* finderz_xattr_get_all_finderz (const gchar *path);

/* High-level functions for specific metadata */

/* Rating (1-5 stars) */
gint finderz_xattr_get_rating (const gchar *path);
gboolean finderz_xattr_set_rating (const gchar *path, gint rating);

/* AI metadata */
gchar* finderz_xattr_get_ai_prompt (const gchar *path);
gboolean finderz_xattr_set_ai_prompt (const gchar *path, const gchar *prompt);
gchar* finderz_xattr_get_ai_model (const gchar *path);
gboolean finderz_xattr_set_ai_model (const gchar *path, const gchar *model);

/* Copy all Finderz xattrs from one file to another */
gboolean finderz_xattr_copy_all (const gchar *src_path, const gchar *dest_path);

/* Initialize xattr system */
void finderz_xattr_init (void);

G_END_DECLS

#endif /* FINDERZ_XATTR_HANDLER_H */