#pragma once

#include <glib-object.h>

typedef enum
{
    CLIPPOR_INSTANCE_TYPE_WAYLAND,
    CLIPPOR_INSTANCE_TYPE_X11,
    CLIPPOR_INSTANCE_TYPE_NONE
} ClipporInstanceType;

#define CLIPPOR_TYPE_INSTANCE_TYPE (clippor_instance_type_get_type())
GType clippor_instance_type_get_type(void);

#define CLIPPOR_INSTANCE_ERROR (clippor_instance_error_quark())

typedef enum
{
    CLIPPOR_INSTANCE_ERROR_INIT
} ClipporInstanceError;

GQuark clippor_instance_error_quark(void);

#define CLIPPOR_TYPE_INSTANCE (clippor_instance_get_type())

G_DECLARE_FINAL_TYPE(
    ClipporInstance, clippor_instance, CLIPPOR, INSTANCE, GObject
)
