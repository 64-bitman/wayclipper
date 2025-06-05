#pragma once

#include <glib-object.h>
#include <wayland-client.h>

#define WAYLAND_CONNECTION_ERROR (wayland_connection_error_quark())

typedef enum
{
    WAYLAND_CONNECTION_ERROR_CONNECT
} WaylandConnectionError;

GQuark wayland_connection_error_quark(void);

#define WAYLAND_TYPE_CONNECTION (wayland_connection_get_type())

G_DECLARE_FINAL_TYPE(
    WaylandConnection, wayland_connection, WAYLAND, CONNECTION, GObject
)
