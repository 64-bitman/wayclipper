#include "util.h"
#include "types.h"
#include "config.h"
#include "ext-data-control-unstable-v1.h"
#include "wlr-data-control-unstable-v1.h"
#include "virtual-keyboard-unstable-v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <wayland-client.h>

#define VERSION "v0.1"

static struct option options[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { "daemon", no_argument, NULL, 'd' },
    { "seat", required_argument, NULL, 's' },
    { "config", required_argument, NULL, 'c' },
};

void help(void);

int main(int argc UNUSED, char **argv UNUSED)
{
    int c, i;

    // Commandline flags
    bool daemon = false;
    char *seat_name = NULL, *config_path = NULL;

    while ((c = getopt_long(argc, argv, "hvds:c:", options, &i)) != -1) {
        switch (c) {
        case 'h':
            help();
            return EXIT_SUCCESS;
        case 'v':
            LOG("wayclipper version " VERSION);
            return EXIT_SUCCESS;
        case 'd':
            daemon = true;
            break;
        case 's':
            free(seat_name);
            seat_name = strdup(optarg);
            break;
        case 'c':
            free(config_path);
            config_path = strdup(optarg);
            break;
        case '?':
            return EXIT_FAILURE;
        default:
            LOG("getopt error %o", c);
            return EXIT_FAILURE;
        }
    }

    if (daemon) {
        LOG("starting wayclipper daemon...");

        struct wayland_connection *ct = init_wayland_onnection(NULL);

        if (ct == NULL) {
            LOG("failed connecting to wayland display");
            return EXIT_FAILURE;
        }

        struct config *cfg = parse_config(config_path);

        if (cfg == NULL) {
            LOG("failed parsing configuration file");
            return EXIT_FAILURE;
        }

        set_seat(ct, seat_name);

        uninit_wayland_connection(ct);
        free_config(cfg);
    }

    free(seat_name);
    free(config_path);

    return EXIT_SUCCESS;
}

void help(void)
{
    printf("wayclipper version " VERSION "\n");
}
