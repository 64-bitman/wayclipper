#include "config.h"
#include "tomlc17.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

// Default configuration
static const char *default_cfg = "[history]\n"
                                 "max_entries = 100\n"
                                 "max_size_mb = 10\n";

// Represents an option in the toml config file
struct toml_option {
    const char *tab;
    const char *name;
    toml_datum_t *data;
    toml_type_t type;
};

static char *get_config_path(const char *path);

/*
 * Parse the config and return the config structure. if path is NULL then use
 * then check predefined locations for the config file, else use default values.
 * Returns NULL on failure.
 */
struct config *parse_config(const char *path)
{
    char *cfg_path = get_config_path(path);
    struct config *cfg = calloc(1, sizeof(*cfg));

    if (cfg == NULL)
        return NULL;

    toml_result_t final, def, result = { 0 };

    // Set defaults
    def = toml_parse(default_cfg, strlen(default_cfg));

    if (cfg_path == NULL) {
        // Use default values
        free(cfg_path);
        final = def;
        memset(&def, 0, sizeof(def)); // Prevent double free
        goto set_opts;
    }
    cfg->path = cfg_path;

    // Parse config file
    result = toml_parse_file_ex(cfg_path);

    if (!result.ok) {
        LOG("failed parsing config file %s -> %s", cfg_path, result.errmsg);
        toml_free(result);
        toml_free(def);
        free_config(cfg);
        return NULL;
    }

    // Override defaults with user choices
    final = toml_merge(&def, &result);

set_opts: { // Remove annoying C23 warning
}

    struct toml_option opts[] = {
        { "history", "max_entries", &cfg->max_entries, TOML_INT64 },
        { "history", "max_size_mb", &cfg->max_size_mb, TOML_INT64 }
    };

    toml_datum_t tab;
    const char *prev_tab = NULL;

    for (size_t i = 0; i < sizeof(opts) / sizeof(*opts); i++) {
        struct toml_option *opt = &opts[i];

        // Set table. If previous table is same as current, then don't
        // redundantly call toml_get() again.
        if (prev_tab == NULL || strcmp(prev_tab, opt->tab) != 0) {
            prev_tab = opt->tab;
            tab = toml_get(final.toptab, opt->tab);
        }
        toml_datum_t data = toml_get(tab, opt->name);

        if (data.type == opt->type)
            *opt->data = data;
        else {
            LOG("config error: %s is invalid, using default", opt->name);
            *opt->data = toml_get(toml_get(def.toptab, opt->tab), opt->name);
        }
    }

    toml_free(final);
    toml_free(result);
    toml_free(def);

    return cfg;
}

/*
 * Free config structure
 */
void free_config(struct config *cfg)
{
    if (cfg == NULL)
        return;

    free(cfg->path);
    free(cfg);
}

/*
 * Get the config file path, returns NULL if an error occured or no config file
 * found.
 */
static char *get_config_path(const char *path)
{
    char *cfg_path = malloc(PATH_MAX);

    if (cfg_path == NULL)
        return NULL;

    // Check $XDG_CONFIG_HOME/wayclipper/, else ~/.config/wayclipper/, else
    // try ~/.wayclipper/
    if (path == NULL) {
        char *env;

        cfg_path[0] = 0;

        // Check $XDG_CONFIG_HOME/wayclipper/
        if ((env = getenv("XDG_CONFIG_HOME")) != NULL)
            snprintf(cfg_path, PATH_MAX, "%s/wayclipper/config.toml", env);

        if (access(cfg_path, R_OK) == OK)
            return cfg_path;

        // Check $HOME/.config/wayclipper/
        if ((env = getenv("HOME")) != NULL)
            snprintf(cfg_path, PATH_MAX, "%s/.config/wayclipper/config.toml",
                     env);

        if (access(cfg_path, R_OK) == OK)
            return cfg_path;

        // Check $HOME/.wayclipper/
        if (env != NULL)
            snprintf(cfg_path, PATH_MAX, "%s/.wayclipper/config.toml", env);

        if (access(cfg_path, R_OK) == OK)
            return cfg_path;

        // None found
        free(cfg_path);
        return NULL;
    } else
        snprintf(cfg_path, PATH_MAX, "%s", path);

    return cfg_path;
}
