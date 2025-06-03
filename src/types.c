#include "util.h"
#include "types.h"
#include "ext-data-control-unstable-v1.h"
#include "wlr-data-control-unstable-v1.h"
#include "virtual-keyboard-unstable-v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wayland-client.h>

static void registry_listener_event_global(void *data,
                                           struct wl_registry *registry,
                                           uint32_t name, const char *interface,
                                           uint32_t version);
static void registry_listener_event_global_remove(void *data,
                                                  struct wl_registry *registry,
                                                  uint32_t name);

static void seat_listener_event_name(void *data, struct wl_seat *seat,
                                     const char *name);
static void seat_listener_event_capabilities(void *data, struct wl_seat *seat,
                                             uint32_t capabilities);

static struct wl_registry_listener registry_listener = {
    .global = registry_listener_event_global,
    .global_remove = registry_listener_event_global_remove
};

static struct wl_seat_listener seat_listener = {
    .name = seat_listener_event_name,
    .capabilities = seat_listener_event_capabilities
};

/*
 * Connect to wayland display and initialize globals.
 */
struct wayland_connection *init_wayland_onnection(const char *name)
{
    struct wayland_connection *ct = calloc(1, sizeof(*ct));

    if (ct == NULL)
        goto fail;

    ct->display.proxy = wl_display_connect(name);

    if (ct->display.proxy == NULL)
        goto fail;

    if (name == NULL)
        name = getenv("WAYLAND_DISPLAY");

    if (name != NULL)
        ct->display.name = strdup(name);

    if (ct->display.name == NULL)
        goto fail;

    ct->display.fd = wl_display_get_fd(ct->display.proxy);

    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);
    wl_registry_add_listener(ct->registry.proxy, &registry_listener, ct);

    wl_array_init(&ct->gobjects.seats);

    if (wl_display_roundtrip(ct->display.proxy) == FAIL)
        goto fail;

    return ct;
fail:
    LOG("failed initializing wayland connection");
    uninit_wayland_connection(ct);

    return NULL;
}

#define gobject_destroy(manager)      \
    if (ct->gobjects.manager != NULL) \
        manager##_destroy(ct->gobjects.manager);

/*
 * Destroy all objects and resources and free the connection structure itself.
 */
void uninit_wayland_connection(struct wayland_connection *ct)
{
    if (ct == NULL)
        return;

    gobject_destroy(ext_data_control_manager_v1);
    gobject_destroy(zwlr_data_control_manager_v1);
    gobject_destroy(zwp_virtual_keyboard_manager_v1);

    struct seat *seat;

    wl_array_for_each(seat, &ct->gobjects.seats) free_seat_contents(seat);
    wl_array_release(&ct->gobjects.seats);

    if (ct->registry.proxy != NULL)
        wl_registry_destroy(ct->registry.proxy);

    if (ct->display.proxy != NULL) {
        wl_display_disconnect(ct->display.proxy);
        free(ct->display.name);
    }

    free(ct);
}

#define registry_bind(manager, min_ver)                                      \
    if (strcmp(interface, manager##_interface.name) == 0 &&                  \
        version >= min_ver) {                                                \
        ct->gobjects.manager =                                               \
            wl_registry_bind(registry, name, &manager##_interface, version); \
        return;                                                              \
    }

/*
 * Global event for registry
 */
static void registry_listener_event_global(void *data,
                                           struct wl_registry *registry,
                                           uint32_t name, const char *interface,
                                           uint32_t version)
{
    struct wayland_connection *ct = data;

    // Bind to global objects
    registry_bind(ext_data_control_manager_v1, 1);
    registry_bind(zwlr_data_control_manager_v1, 1);
    registry_bind(zwp_virtual_keyboard_manager_v1, 1);

    // Add seat to special array
    if (strcmp(interface, wl_seat_interface.name) == 0 && version >= 2) {
        struct wl_seat *seat =
            wl_registry_bind(registry, name, &wl_seat_interface, version);
        struct seat *store = wl_array_add(&ct->gobjects.seats, sizeof(*store));

        if (store == NULL) {
            wl_seat_destroy(seat);
            return;
        }
        store->proxy = seat;

        // Get name and capabilities
        wl_seat_add_listener(seat, &seat_listener, store);

        if (wl_display_roundtrip(ct->display.proxy) == FAIL)
            free_seat_contents(store);

        // Check if seats have been allocated correctly
        wl_array_for_each(store, &ct->gobjects.seats)
        {
            if (store->name == NULL)
                // Just destroy seat proxy, we will check if name is NULL when
                // we try using it.
                free_seat_contents(store);
        }

        return;
    }
}

/*
 * When global is removed for registry. Don't do anything to avoid race
 * conditions.
 */
static void
registry_listener_event_global_remove(void *data UNUSED,
                                      struct wl_registry *registry UNUSED,
                                      uint32_t name UNUSED)
{
}

/*
 * Assign seat name
 */
static void seat_listener_event_name(void *data, struct wl_seat *seat UNUSED,
                                     const char *name)
{
    struct seat *store = data;

    store->name = strdup(name);
}

/*
 * Assign capabilities of seat
 */
static void seat_listener_event_capabilities(void *data,
                                             struct wl_seat *seat UNUSED,
                                             uint32_t capabilities)
{
    struct seat *store = data;

    store->capabilities = capabilities;
}

/*
 * Destroy the seat proxy and free the seat name string. Does not free the seat
 * structure itself.
 */
void free_seat_contents(struct seat *seat)
{
    if (seat->proxy != NULL)
        wl_seat_destroy(seat->proxy);
    if (seat->name != NULL)
        free(seat->name);
}

/*
 * Return seat with given name from connection, if name is NULL or empty then
 * try using $XDG_SEAT else use the first one found. If none is found then
 * return NULL.
 */
struct seat *get_seat(struct wayland_connection *ct, const char *name)
{
    bool getany = false;

    if (name == NULL || strcmp(name, "")) {
        const char *xdg_seat = getenv("XDG_SEAT");

        if (xdg_seat == NULL)
            getany = true;
        else
            name = xdg_seat;
    }

    struct seat *seat;

    wl_array_for_each(seat, &ct->gobjects.seats)
    {
        if (seat->name != NULL)
            if (getany || strcmp(seat->name, name) == 0)
                break;
    }

    return seat;
}

/*
 * Set the current seat for connection to `seat_name`. If an error occurs when
 * FAIL is returned, else OK.
 */
ret_T set_seat(struct wayland_connection *ct, const char *seat_name)
{
    struct seat *seat = get_seat(ct, seat_name);

    if (seat == NULL)
        return FAIL;

    ct->cur_seat = seat;

    return OK;
}
