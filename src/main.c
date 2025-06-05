#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

static gboolean opt_version = FALSE;
static gboolean opt_server = FALSE;
static gboolean opt_debug = FALSE;

static GOptionEntry help_entries[] = {
    {"version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, "Print version", NULL},
    {"server", 's', 0, G_OPTION_ARG_NONE, &opt_server, "Serve the clipboard",
     NULL},
    {"debug", 'd', 0, G_OPTION_ARG_NONE, &opt_debug, "Enable verbose logging",
     NULL},
    G_OPTION_ENTRY_NULL
};

int
main(int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *context = g_option_context_new("- clipboard manager");

    // Read commandline
    g_option_context_add_main_entries(context, help_entries, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_option_context_free(context);
        goto exit;
    }
    g_option_context_free(context);

    // Serve some command line flags
    if (opt_version)
    {
        g_print("Clipstuff version " VERSION "\n");
        goto exit;
    }
    if (opt_debug)
        g_log_set_debug_enabled(TRUE);

    if (opt_server)
    {
        // Serve clipboard
        GMainContext *main_context = g_main_context_new();
        GMainLoop *main_loop = g_main_loop_new(main_context, FALSE);
        GSettings *settings = g_settings_new("com.github.64bitman.clippor");

        g_main_loop_run(main_loop);

        g_object_unref(settings);
        g_main_loop_unref(main_loop);
        g_main_context_unref(main_context);
    }

exit:
    if (error != NULL)
    {
        g_critical("%s\n", error->message);
        g_error_free(error);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
