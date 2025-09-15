/* finderz-integration.h
 * 
 * Integration points for Finderz features in Nemo
 */

#ifndef FINDERZ_INTEGRATION_H
#define FINDERZ_INTEGRATION_H

#include <glib.h>

G_BEGIN_DECLS

/* Initialize Finderz features */
void finderz_init (void);

/* Check if a directory has Mac metadata */
gboolean finderz_directory_has_mac_metadata (const gchar *directory_path);

/* Get view style from DS_Store if available */
gint finderz_get_mac_view_style (const gchar *directory_path);

/* Cleanup Finderz features */
void finderz_cleanup (void);

G_END_DECLS

#endif /* FINDERZ_INTEGRATION_H */