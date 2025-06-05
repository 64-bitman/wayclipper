#include "wayland-connection.h"
#include "ext-data-control-v1.h"
#include "virtual-keyboard-unstable-v1.h"
#include "wlr-data-control-unstable-v1.h"
#include <glib-object.h>
#include <wayland-client.h>

G_DEFINE_QUARK(wayland_connection_error_quark, wayland_connection_error)

struct _WaylandConnection
{
    GObject parent;

    struct
    {
        struct wl_display *proxy;
        gchar *name;
        gint fd;
    } display;

    struct
    {
        struct wl_registry *proxy;
    } registry;

    struct
    {
        GPtrArray *seats;

        struct ext_data_control_manager_v1 *ext_dc_manager;
        struct zwlr_data_control_manager_v1 *wlr_dc_manager;
        struct zwp_virtual_keyboard_manager_v1 *virtkb_manager;
    } gobjects;
};

G_DEFINE_TYPE(WaylandConnection, wayland_connection, G_TYPE_OBJECT)

typedef enum
{
    PROP_DISPLAY = 1,
    PROP_SEATS,
    N_PROPERTIES
} WaylandConnectionProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {
    NULL,
};

static void
wayland_connection_set_property(
    GObject *object, guint property_id, const GValue *value, GParamSpec *pspec
)
{
    WaylandConnection *self = WAYLAND_CONNECTION(object);

    switch ((WaylandConnectionProperty)property_id)
    {
    case PROP_DISPLAY:
        self->display.name = g_value_dup_string(value);
        // Reinitialize connection...
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
wayland_connection_get_property(
    GObject *object, guint property_id, GValue *value, GParamSpec *pspec
)
{
    WaylandConnection *self = WAYLAND_CONNECTION(object);

    switch ((WaylandConnectionProperty)property_id)
    {
    case PROP_DISPLAY:
        g_value_set_string(value, self->display.name);
        break;
    case PROP_SEATS:
        g_value_set_boxed(value, self->gobjects.seats);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
wayland_connection_dispose(GObject *object)
{
    WaylandConnection *self = WAYLAND_CONNECTION(object);

    if (self->gobjects.seats != NULL)
    {
        g_ptr_array_unref(self->gobjects.seats);
        self->gobjects.seats = NULL;
    }

    G_OBJECT_CLASS(wayland_connection_parent_class)->dispose(object);
}

static void
wayland_connection_finalize(GObject *object)
{
    WaylandConnection *self = WAYLAND_CONNECTION(object);

    g_free(self->display.name);

    // uninit connection...

    G_OBJECT_CLASS(wayland_connection_parent_class)->finalize(object);
}

static void
wayland_connection_class_init(WaylandConnectionClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(class);

    gobject_class->set_property = wayland_connection_set_property;
    gobject_class->get_property = wayland_connection_get_property;

    gobject_class->dispose = wayland_connection_dispose;
    gobject_class->finalize = wayland_connection_finalize;

    obj_properties[PROP_DISPLAY] = g_param_spec_string(
        "display", "Display name", "Name of connected Wayland display", "",
        G_PARAM_READWRITE
    );
    obj_properties[PROP_SEATS] = g_param_spec_boxed(
        "seats", "Seats", "List of seats available for Wayland display",
        G_TYPE_PTR_ARRAY, G_PARAM_READABLE
    );

    g_object_class_install_properties(
        gobject_class, N_PROPERTIES, obj_properties
    );
}

static void
wayland_connection_init(WaylandConnection *self)
{
    self->gobjects.seats = g_ptr_array_new();
}
