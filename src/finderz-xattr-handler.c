/* finderz-xattr-handler.c
 * 
 * Extended attributes handler for indelible metadata storage
 * Makes metadata survive file operations and transfers
 */

#include <glib.h>
#include <gio/gio.h>
#include <sys/xattr.h>
#include <errno.h>
#include <string.h>

#define FINDERZ_XATTR_PREFIX "user.finderz."
#define FINDERZ_XATTR_RATING FINDERZ_XATTR_PREFIX "rating"
#define FINDERZ_XATTR_AI_PREFIX FINDERZ_XATTR_PREFIX "ai."
#define FINDERZ_XATTR_EXIF_PREFIX FINDERZ_XATTR_PREFIX "exif."
#define FINDERZ_XATTR_MAC_PREFIX FINDERZ_XATTR_PREFIX "mac."

/* Check if filesystem supports extended attributes */
gboolean
finderz_xattr_supported (const gchar *path)
{
    char value[1];
    ssize_t ret = listxattr (path, value, sizeof(value));
    
    /* If it returns >= 0 or ERANGE (buffer too small), xattrs are supported */
    return (ret >= 0 || errno == ERANGE);
}

/* List all extended attributes for a file */
GList*
finderz_xattr_list (const gchar *path, GError **error)
{
    GList *attrs = NULL;
    ssize_t bufsize, keysize;
    char *buf, *key;
    
    /* Get required buffer size */
    bufsize = listxattr (path, NULL, 0);
    if (bufsize < 0) {
        if (errno == ENOTSUP) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                         "Extended attributes not supported on this filesystem");
        } else {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "Failed to list extended attributes: %s", g_strerror (errno));
        }
        return NULL;
    }
    
    if (bufsize == 0) {
        return NULL; /* No attributes */
    }
    
    /* Allocate buffer and get list */
    buf = g_malloc (bufsize);
    keysize = listxattr (path, buf, bufsize);
    if (keysize < 0) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to list extended attributes: %s", g_strerror (errno));
        g_free (buf);
        return NULL;
    }
    
    /* Parse null-terminated list */
    key = buf;
    while (key < buf + keysize) {
        attrs = g_list_append (attrs, g_strdup (key));
        key += strlen (key) + 1;
    }
    
    g_free (buf);
    return attrs;
}

/* Get value of an extended attribute */
gchar*
finderz_xattr_get (const gchar *path, const gchar *name, GError **error)
{
    ssize_t bufsize, valsize;
    char *value;
    
    /* Get required buffer size */
    bufsize = getxattr (path, name, NULL, 0);
    if (bufsize < 0) {
        if (errno == ENOATTR || errno == ENODATA) {
            return NULL; /* Attribute doesn't exist */
        }
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to get extended attribute %s: %s", 
                     name, g_strerror (errno));
        return NULL;
    }
    
    /* Allocate buffer and get value */
    value = g_malloc (bufsize + 1);
    valsize = getxattr (path, name, value, bufsize);
    if (valsize < 0) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to get extended attribute %s: %s",
                     name, g_strerror (errno));
        g_free (value);
        return NULL;
    }
    
    value[valsize] = '\0';
    return value;
}

/* Set value of an extended attribute */
gboolean
finderz_xattr_set (const gchar *path, const gchar *name, 
                   const gchar *value, GError **error)
{
    int ret = setxattr (path, name, value, strlen (value), 0);
    if (ret < 0) {
        g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                     "Failed to set extended attribute %s: %s",
                     name, g_strerror (errno));
        return FALSE;
    }
    return TRUE;
}

/* Remove an extended attribute */
gboolean
finderz_xattr_remove (const gchar *path, const gchar *name, GError **error)
{
    int ret = removexattr (path, name);
    if (ret < 0) {
        if (errno != ENOATTR && errno != ENODATA) {
            g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                         "Failed to remove extended attribute %s: %s",
                         name, g_strerror (errno));
            return FALSE;
        }
    }
    return TRUE;
}

/* Get all Finderz-specific attributes as a hash table */
GHashTable*
finderz_xattr_get_all_finderz (const gchar *path)
{
    GHashTable *attrs = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                g_free, g_free);
    GList *all_attrs = finderz_xattr_list (path, NULL);
    GList *l;
    
    for (l = all_attrs; l != NULL; l = l->next) {
        const gchar *name = l->data;
        
        /* Only get our attributes */
        if (g_str_has_prefix (name, FINDERZ_XATTR_PREFIX)) {
            gchar *value = finderz_xattr_get (path, name, NULL);
            if (value) {
                /* Strip prefix for cleaner keys */
                const gchar *key = name + strlen (FINDERZ_XATTR_PREFIX);
                g_hash_table_insert (attrs, g_strdup (key), value);
            }
        }
    }
    
    g_list_free_full (all_attrs, g_free);
    return attrs;
}

/* High-level functions for specific metadata */

/* Get/Set rating (1-5 stars) */
gint
finderz_xattr_get_rating (const gchar *path)
{
    gchar *value = finderz_xattr_get (path, FINDERZ_XATTR_RATING, NULL);
    gint rating = 0;
    
    if (value) {
        rating = CLAMP (atoi (value), 0, 5);
        g_free (value);
    }
    
    return rating;
}

gboolean
finderz_xattr_set_rating (const gchar *path, gint rating)
{
    gchar value[2];
    rating = CLAMP (rating, 0, 5);
    g_snprintf (value, sizeof (value), "%d", rating);
    return finderz_xattr_set (path, FINDERZ_XATTR_RATING, value, NULL);
}

/* Get/Set AI metadata */
gchar*
finderz_xattr_get_ai_prompt (const gchar *path)
{
    return finderz_xattr_get (path, FINDERZ_XATTR_AI_PREFIX "prompt", NULL);
}

gboolean
finderz_xattr_set_ai_prompt (const gchar *path, const gchar *prompt)
{
    return finderz_xattr_set (path, FINDERZ_XATTR_AI_PREFIX "prompt", prompt, NULL);
}

gchar*
finderz_xattr_get_ai_model (const gchar *path)
{
    return finderz_xattr_get (path, FINDERZ_XATTR_AI_PREFIX "model", NULL);
}

gboolean
finderz_xattr_set_ai_model (const gchar *path, const gchar *model)
{
    return finderz_xattr_set (path, FINDERZ_XATTR_AI_PREFIX "model", model, NULL);
}

/* Copy all Finderz xattrs from one file to another */
gboolean
finderz_xattr_copy_all (const gchar *src_path, const gchar *dest_path)
{
    GHashTable *attrs = finderz_xattr_get_all_finderz (src_path);
    GHashTableIter iter;
    gpointer key, value;
    gboolean success = TRUE;
    
    g_hash_table_iter_init (&iter, attrs);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        gchar *full_name = g_strdup_printf ("%s%s", FINDERZ_XATTR_PREFIX, (char*)key);
        if (!finderz_xattr_set (dest_path, full_name, value, NULL)) {
            success = FALSE;
        }
        g_free (full_name);
    }
    
    g_hash_table_destroy (attrs);
    return success;
}

/* Initialize xattr system - check support, set up monitoring */
void
finderz_xattr_init (void)
{
    /* Test xattr support on common locations */
    if (finderz_xattr_supported (g_get_home_dir ())) {
        g_debug ("FINDERZ: Extended attributes supported in home directory");
    } else {
        g_warning ("FINDERZ: Extended attributes not supported in home directory");
    }
    
    /* Register with file operations to preserve xattrs on copy/move */
    /* TODO: Hook into Nemo's file operations */
}