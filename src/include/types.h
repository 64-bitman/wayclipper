#pragma once

#include "util.h"
#include "ext-data-control-unstable-v1.h"
#include "wlr-data-control-unstable-v1.h"
#include "virtual-keyboard-unstable-v1.h"
#include <wayland-client.h>

struct seat {
    struct wl_seat *proxy;
    char *name;
    uint32_t capabilities;
};

// Represents a wayland connection and its global objects.
struct wayland_connection {
    struct {
        struct wl_display *proxy;
        char *name;
        int fd;
    } display;

    struct {
        struct wl_registry *proxy;
    } registry;

    struct {
        struct wl_array seats;

        struct ext_data_control_manager_v1 *ext_data_control_manager_v1;
        struct zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;

        struct zwp_virtual_keyboard_manager_v1 *zwp_virtual_keyboard_manager_v1;
    } gobjects;

    struct seat *cur_seat;
};

struct wayland_connection *init_wayland_onnection(const char *name);
void uninit_wayland_connection(struct wayland_connection *ct);

void free_seat_contents(struct seat *seat);
struct seat *get_seat(struct wayland_connection *ct, const char *name);
ret_T set_seat(struct wayland_connection *ct, const char *seat_name);
