#include "clippor-entry.h"
#include <gio/gio.h>
#include <glib-object.h>

G_DEFINE_QUARK(clippor_entry_error_quark, clippor_entry_error)

struct _ClipporEntry
{
    GObject parent;

    guint64 index;          // Index in the history list
    GFile *file;            // Persistent file that stores the data
    GHashTable *mime_types; // Hash table of mime types. Each value is a pointer
                            // Bytes GObject of the data it represents. Value
                            // may be NULL is save memory. GFile *file;
                            // // Persistent file that stores the data
};

G_DEFINE_TYPE(ClipporEntry, clippor_entry, G_TYPE_OBJECT)

typedef enum
{
    PROP_INDEX = 1,
    PROP_MIME_TYPES,
    PROP_FILE,
    N_PROPERTIES
} ClipporEntryProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {
    NULL,
};

static void
clippor_entry_set_property(
    GObject *object, guint property_id, const GValue *value, GParamSpec *pspec
)
{
    ClipporEntry *self = CLIPPOR_ENTRY(object);

    switch ((ClipporEntryProperty)property_id)
    {
    case PROP_INDEX:
        self->index = g_value_get_uint64(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
clippor_entry_get_property(
    GObject *object, guint property_id, GValue *value, GParamSpec *pspec
)
{
    ClipporEntry *self = CLIPPOR_ENTRY(object);

    switch ((ClipporEntryProperty)property_id)
    {
    case PROP_INDEX:
        g_value_set_uint64(value, self->index);
        break;
    case PROP_FILE:
        g_value_set_object(value, self->file);
        break;
    case PROP_MIME_TYPES:
        g_value_set_boxed(value, self->mime_types);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
clippor_entry_dispose(GObject *object)
{
    ClipporEntry *self = CLIPPOR_ENTRY(object);

    g_clear_object(&self->file);

    if (self->mime_types != NULL)
    {
        g_hash_table_unref(self->mime_types);
        self->mime_types = NULL;
    }

    G_OBJECT_CLASS(clippor_entry_parent_class)->dispose(object);
}

static void
clippor_entry_finalize(GObject *object)
{
    ClipporEntry *self G_GNUC_UNUSED = CLIPPOR_ENTRY(object);

    G_OBJECT_CLASS(clippor_entry_parent_class)->finalize(object);
}

static void
clippor_entry_class_init(ClipporEntryClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(class);

    gobject_class->set_property = clippor_entry_set_property;
    gobject_class->get_property = clippor_entry_get_property;

    gobject_class->dispose = clippor_entry_dispose;
    gobject_class->finalize = clippor_entry_finalize;

    obj_properties[PROP_INDEX] = g_param_spec_uint64(
        "index", "Index", "Position in the history list", 0, G_MAXUINT64, 0,
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
    );
    obj_properties[PROP_MIME_TYPES] = g_param_spec_boxed(
        "mime-types", "Mime types",
        "List of mime types that can be represented", G_TYPE_HASH_TABLE,
        G_PARAM_READABLE
    );
    obj_properties[PROP_FILE] = g_param_spec_object(
        "file", "File", "Persistent file holding all data", G_TYPE_FILE,
        G_PARAM_READABLE
    );

    g_object_class_install_properties(
        gobject_class, N_PROPERTIES, obj_properties
    );
}

static void
clippor_entry_init(ClipporEntry *self)
{
    self->mime_types = g_hash_table_new(g_str_hash, g_str_equal);
}

gboolean
clippor_entry_set_file(ClipporEntry *self, GFile *file, GError **error)
{
    g_return_val_if_fail(CLIPPOR_IS_ENTRY(self), FALSE);

    if (self->file != NULL)
    {
        g_set_error(
            error, CLIPPOR_ENTRY_ERROR, CLIPPOR_ENTRY_ERROR_SET,
            "File already set for Entry #%ld", self->index
        );
        return FALSE;
    }

    self->file = g_object_ref(file);
    return TRUE;
}

void
clippor_entry_add_mime_type(
    ClipporEntry *self, gchar *mime_type, gpointer data, gsize size
)
{
    g_return_if_fail(CLIPPOR_IS_ENTRY(self));

    GBytes *bytes = NULL;

    if (data != NULL && size > 0)
        bytes = g_bytes_new(data, size);

    g_hash_table_insert(self->mime_types, mime_type, bytes);
}
