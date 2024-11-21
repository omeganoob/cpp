#include <iostream>
#include <dbus/dbus.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

constexpr const char* SERVER_BUS_NAME = "chung.simple";
constexpr const char* SERVER_INTERFACE = "chung.simple.signal";
constexpr const char* SIGNAL_NAME = "chung_signal";
constexpr const char* CATCH_RULE = "type='signal',interface='chung.simple.signal'";

void handle_message(DBusConnection* connection, DBusError& error) {
    while (true) {  // Process all pending messages
        DBusMessage* message = dbus_connection_pop_message(connection);
        if (!message) {
            break;  // No more messages
        }

        if (dbus_message_is_signal(message, SERVER_INTERFACE, SIGNAL_NAME)) {
            const char* received_message = nullptr;
            if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &received_message, DBUS_TYPE_INVALID)) {
                std::cout << "Received signal with message: " << received_message << std::endl;
            } else {
                std::cerr << "Failed to get message arguments." << std::endl;
                dbus_error_free(&error);
            }
        }
        dbus_message_unref(message);
    }
}

void listen_dbus_signal() {
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

    // Add a match rule for filtering incoming signals
    dbus_bus_add_match(connection, CATCH_RULE, &error);
    dbus_connection_flush(connection);

    if (dbus_error_is_set(&error)) {
        std::cerr << "Match Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return;
    }

    // Get the file descriptor for the connection
    int fd;
    if (!dbus_connection_get_unix_fd(connection, &fd)) {
        std::cerr << "Failed to get connection file descriptor" << std::endl;
        return;
    }

    // Set up polling
    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    std::cout << "Listening for D-Bus signals..." << std::endl;

    while (true) {
        // Wait for data to be available
        int ret = poll(fds, 1, -1);  // -1 means wait indefinitely

        if (ret < 0) {
            if (errno == EINTR) {
                continue;  // Interrupted by signal, try again
            }
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }

        if (ret > 0 && (fds[0].revents & POLLIN)) {
            // Non-blocking read of available data
            while (dbus_connection_read_write_dispatch(connection, 0)) {
                handle_message(connection, error);
            }
        }
    }

    // Cleanup
    dbus_connection_unref(connection);
}

int main() {
    listen_dbus_signal();
    return 0;
}