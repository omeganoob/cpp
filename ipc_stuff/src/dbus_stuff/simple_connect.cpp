#include <iostream>
#include <dbus/dbus.h>

constexpr char* BUS_NAME = "chung.simple";
constexpr char* INTERFACE_NAME = "chung.simple.connect";
constexpr char* OBJECT_PATH_NAME = "/chung/simple/connector";

int main() {
    // Initialize DBus error handling
    DBusError err;
    dbus_error_init(&err);

    // Connect to the session bus
    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err)) {
        std::cerr << "Connection Error: " << err.message << std::endl;
        dbus_error_free(&err);
        return -1;
    }

    if (!conn) {
        std::cerr << "Failed to connect to the D-Bus session bus." << std::endl;
        return -1;
    }

    std::cout << "Connected to the D-Bus session bus successfully!" << std::endl;


    int return_stt = dbus_bus_request_name(conn, BUS_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);

    if (dbus_error_is_set(&err)) {
        std::cerr << "Connection Error: " << err.message << std::endl;
        dbus_error_free(&err);
        return -1;
    }

    if (return_stt != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        std::cerr << "Dbus: not primary owner, return_stt = " << return_stt << "\n";
        return 1;
    }

    std::cout << "Request well-known name: " << BUS_NAME << std::endl;

    std::cin.get();

    // Cleanup and close connection
    dbus_connection_unref(conn);
    return 0;
}
