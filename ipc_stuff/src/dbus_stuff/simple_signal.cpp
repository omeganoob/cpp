#include <iostream>
#include <dbus/dbus.h>

constexpr char* BUS_NAME = "chung.simple";
constexpr char* INTERFACE_NAME = "chung.simple.signal";
constexpr char* OBJECT_PATH_NAME = "/chung/simple/signaling";
constexpr char* SIGNAL_NAME = "chung_signal";

void send_dbus_signal() {
    DBusError error;
    dbus_error_init(&error);

    // Connect to the session bus
    DBusConnection* connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Connection Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return;
    }

    if (!connection) {
        std::cerr << "Failed to connect to the D-Bus session bus." << std::endl;
        return;
    }

    // Request a well-known name on the bus
    int ret = dbus_bus_request_name(connection, BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Name Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        std::cerr << "Failed to get the primary ownership of the name." << std::endl;
        return;
    }

    // Create a signal message
    DBusMessage* message = dbus_message_new_signal(
        OBJECT_PATH_NAME,
        INTERFACE_NAME,
        SIGNAL_NAME
    );

    if (!message) {
        std::cerr << "Message Error: Failed to create signal message." << std::endl;
        return;
    }

    // Append arguments to the signal (optional, here we send a simple string)
    const char* message_content = "CHUNG DEP ZAI HEHEHEHEHE";
    
    if (!dbus_message_append_args(message, DBUS_TYPE_STRING, &message_content, DBUS_TYPE_INVALID)) {
        std::cerr << "Message Error: Failed to append arguments." << std::endl;
        dbus_message_unref(message);
        return;
    }

    // Send the signal
    if (!dbus_connection_send(connection, message, nullptr)) {
        std::cerr << "Message Error: Failed to send signal." << std::endl;
    } else {
        std::cout << "Signal sent successfully!" << std::endl;
    }

    // Clean up
    dbus_message_unref(message);
    dbus_connection_flush(connection);
    dbus_connection_unref(connection);
}

int main() {
    send_dbus_signal();
    return 0;
}
