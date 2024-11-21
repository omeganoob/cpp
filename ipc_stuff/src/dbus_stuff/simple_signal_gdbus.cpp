#include <gio/gio.h>
#include <glib.h>

constexpr char* BUS_NAME = "chung.simple";
constexpr char* INTERFACE_NAME = "chung.simple.signal";
constexpr char* OBJECT_PATH =  "/chung/simple/signaling";
constexpr char* SIGNAL_NAME = "chung_signal";

static gboolean emit_signal(gpointer user_data) {
    GDBusConnection *connection = G_DBUS_CONNECTION(user_data);
    GError *error = NULL;

    // Emit the signal on the D-Bus
    if (!g_dbus_connection_emit_signal(
            connection,                  // Connection to use
            NULL,                        // Destination (NULL means broadcast)
            OBJECT_PATH,                 // Object path
            INTERFACE_NAME,              // Interface name
            SIGNAL_NAME,                // Signal name
            g_variant_new("(s)", "[Gdbus] CHUNG DEP ZAI HEHEHEHE!"), // Signal arguments
            &error)) {
        g_printerr("Error emitting signal: %s\n", error->message);
        g_error_free(error);
        return G_SOURCE_REMOVE;  // Stop the timer if an error occurred
    }

    g_print("Signal emitted successfully.\n");
    return G_SOURCE_CONTINUE;  // Continue emitting signal every second
}

int main() {
    GMainLoop *loop;
    GDBusConnection *connection;
    GError *error = NULL;

    // Create the main event loop
    loop = g_main_loop_new(NULL, FALSE);

    // Connect to the session bus
    connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (error != NULL) {
        g_printerr("Failed to connect to the D-Bus session bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Request a well-known name on the session bus
    guint owner_id = g_bus_own_name_on_connection(
        connection,
        BUS_NAME,
        G_BUS_NAME_OWNER_FLAGS_NONE,
        NULL, NULL, NULL, NULL
    );

    // Emit the signal every second
    g_timeout_add_seconds(1, emit_signal, connection);

    g_print("Signal server running...\n");

    // Run the main loop
    g_main_loop_run(loop);

    // Cleanup
    g_bus_unown_name(owner_id);
    g_object_unref(connection);
    g_main_loop_unref(loop);

    return 0;
}
