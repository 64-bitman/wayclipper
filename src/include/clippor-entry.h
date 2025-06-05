#pragma once

#include <gio/gio.h>
#include <glib-object.h>

#define CLIPPOR_ENTRY_ERROR (clippor_entry_error_quark())

typedef enum
{
    CLIPPOR_ENTRY_ERROR_SET
} ClipporEntryError;

GQuark clippor_entry_error_quark(void);

#define CLIPPOR_TYPE_ENTRY (clippor_entry_get_type())

G_DECLARE_FINAL_TYPE(ClipporEntry, clippor_entry, CLIPPOR, ENTRY, GObject)

gboolean
clippor_entry_set_file(ClipporEntry *self, GFile *file, GError **error);

void clippor_entry_add_mime_type(
    ClipporEntry *self, gchar *mime_type, gpointer data, gsize size
);
