#pragma once

#include <tomlc17.h>

// Represents a configuration
struct config {
    char *path;

    toml_datum_t max_entries;
    toml_datum_t max_size_mb;
};

struct config *parse_config(const char *path);
void free_config(struct config *cfg);
