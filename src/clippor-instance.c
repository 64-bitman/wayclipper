#include "clippor-instance.h"
#include <glib-object.h>

G_DEFINE_QUARK(clippor_instance_error_quark, clippor_instance_error)

G_DEFINE_ENUM_TYPE(
    ClipporInstanceType, clippor_instance_type,
    G_DEFINE_ENUM_VALUE(CLIPPOR_INSTANCE_TYPE_WAYLAND, "wayland"),
    G_DEFINE_ENUM_VALUE(CLIPPOR_INSTANCE_TYPE_X11, "x11"),
    G_DEFINE_ENUM_VALUE(CLIPPOR_INSTANCE_TYPE_NONE, "none")
)

struct _ClipporInstance
{
    GObject parent;

    ClipporInstanceType type;
    GObject *connection;
};

G_DEFINE_TYPE(ClipporInstance, clippor_instance, G_TYPE_OBJECT)

typedef enum
{
    PROP_TYPE = 1,
    N_PROPERTIES
} ClipporInstanceProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {
    NULL,
};

static void
clippor_instance_set_property(
    GObject *object, guint property_id, const GValue *value, GParamSpec *pspec
)
{
    ClipporInstance *self = CLIPPOR_INSTANCE(object);

    switch ((ClipporInstanceProperty)property_id)
    {
    case PROP_TYPE:
        self->type = g_value_get_enum(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
clippor_instance_get_property(
    GObject *object, guint property_id, GValue *value, GParamSpec *pspec
)
{
    ClipporInstance *self = CLIPPOR_INSTANCE(object);

    switch ((ClipporInstanceProperty)property_id)
    {
    case PROP_TYPE:
        g_value_set_enum(value, self->type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
clippor_instance_dispose(GObject *object)
{
    ClipporInstance *self = CLIPPOR_INSTANCE(object);

    g_clear_object(&self);

    G_OBJECT_CLASS(clippor_instance_parent_class)->dispose(object);
}

static void
clippor_instance_finalize(GObject *object)
{
    ClipporInstance *self G_GNUC_UNUSED = CLIPPOR_INSTANCE(object);

    G_OBJECT_CLASS(clippor_instance_parent_class)->finalize(object);
}

static void
clippor_instance_class_init(ClipporInstanceClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(class);

    gobject_class->set_property = clippor_instance_set_property;
    gobject_class->get_property = clippor_instance_get_property;

    gobject_class->dispose = clippor_instance_dispose;
    gobject_class->finalize = clippor_instance_finalize;

    obj_properties[PROP_TYPE] = g_param_spec_enum(
        "type", "Type", "Type of instance", CLIPPOR_TYPE_INSTANCE_TYPE,
        CLIPPOR_INSTANCE_TYPE_NONE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
    );

    g_object_class_install_properties(
        gobject_class, N_PROPERTIES, obj_properties
    );
}

static void
clippor_instance_init(ClipporInstance *self G_GNUC_UNUSED)
{
    self->type = CLIPPOR_INSTANCE_TYPE_NONE;
}
