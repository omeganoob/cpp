#include <dbus/dbus.h>
#include <iostream>
#include <string>
#include <cstring>

void send_introspection(DBusConnection* connection, DBusMessage* message) {
    const char* introspection_xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
        "<node name=\"/org/example/MediaPlayer\">"
        "    <interface name=\"org.example.MediaPlayer\">"
        "        <method name=\"Play\">"
        "            <arg name=\"uri\" type=\"s\" direction=\"in\"/>"
        "            <arg name=\"result\" type=\"b\" direction=\"out\"/>"
        "        </method>"
        "        <method name=\"Pause\">"
        "            <arg name=\"result\" type=\"b\" direction=\"out\"/>"
        "        </method>"
        "        <method name=\"Stop\">"
        "            <arg name=\"result\" type=\"b\" direction=\"out\"/>"
        "        </method>"
        "        <signal name=\"PlaybackStarted\"/>"
        "        <signal name=\"PlaybackPaused\"/>"
        "        <signal name=\"PlaybackStopped\"/>"
        "        <property name=\"Status\" type=\"s\" access=\"read\"/>"
        "        <property name=\"Volume\" type=\"u\" access=\"readwrite\"/>"
        "    </interface>"
        "</node>";

    DBusMessage* reply = dbus_message_new_method_return(message);
    dbus_message_append_args(reply, DBUS_TYPE_STRING, &introspection_xml, DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, nullptr);
    dbus_message_unref(reply);
}

void handle_method_call(DBusConnection* connection, DBusMessage* message) {
    const char* interface_name = dbus_message_get_interface(message);
    const char* method_name = dbus_message_get_member(message);

    std::cout << "Received method call: " << method_name << " on interface: " << interface_name << std::endl;

    if (strcmp(interface_name, "org.example.MediaPlayer") == 0) {
        if (dbus_message_is_method_call(message, "org.example.MediaPlayer", "Play")) {
            const char* uri;
            dbus_message_get_args(message, nullptr, DBUS_TYPE_STRING, &uri, DBUS_TYPE_INVALID);
            std::cout << "Playing media: " << uri << std::endl;

            DBusMessage* reply = dbus_message_new_method_return(message);
            dbus_bool_t result = true;  // Simulate a successful operation
            dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &result, DBUS_TYPE_INVALID);
            dbus_connection_send(connection, reply, nullptr);
            dbus_message_unref(reply);
        }
        else if (dbus_message_is_method_call(message, "org.example.MediaPlayer", "Pause")) {
            std::cout << "Pausing media." << std::endl;

            DBusMessage* reply = dbus_message_new_method_return(message);
            dbus_bool_t result = true;  // Simulate a successful operation
            dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &result, DBUS_TYPE_INVALID);
            dbus_connection_send(connection, reply, nullptr);
            dbus_message_unref(reply);
        }
        else if (dbus_message_is_method_call(message, "org.example.MediaPlayer", "Stop")) {
            std::cout << "Stopping media." << std::endl;

            DBusMessage* reply = dbus_message_new_method_return(message);
            dbus_bool_t result = true;  // Simulate a successful operation
            dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &result, DBUS_TYPE_INVALID);
            dbus_connection_send(connection, reply, nullptr);
            dbus_message_unref(reply);
        }
        else if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect")) {
            send_introspection(connection, message);
        }
    }
}

int main() {
    DBusError error;
    dbus_error_init(&error);

    // Connect to the session bus
    DBusConnection* connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Connection Error (" << error.name << "): " << error.message << std::endl;
        dbus_error_free(&error);
        return 1;
    }

    // Register the service
    const char* service_name = "org.example.MediaPlayerService";
    dbus_bus_request_name(connection, service_name, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Name Error (" << error.name << "): " << error.message << std::endl;
        dbus_error_free(&error);
        return 1;
    }

    std::cout << "Media Player Service is running..." << std::endl;

    // Main loop
    while (true) {
        // Read messages
        dbus_connection_read_write(connection, 0);
        DBusMessage* message = dbus_connection_pop_message(connection);
        if (message) {
            handle_method_call(connection, message);
            dbus_message_unref(message);
        }
    }

    dbus_connection_unref(connection);
    return 0;
}
