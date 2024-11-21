#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <unistd.h>
#include <cctype>
#include <vector>
#include <string>
#include <string_view>
#include <cstdio>

#include <dbus/dbus.h>

constexpr char* INTERFACE_NAME = "chung.dep.zai";

constexpr char* SERVER_BUS_NAME = "chung.dep.zai.add_server";
constexpr char* SERVER_OBJECT_PATH_NAME = "/chung/dep/zai/adder";
constexpr char* METHOD_NAME = "add_numbers";

constexpr char* CLIENT_BUS_NAME = "chung.dep.zai.add_client";
constexpr char* CLIENT_OBJECT_PATH_NAME = "/chung/dep/zai/add-client";


DBusError dbus_error;
void print_dbus_error(std::string_view err)
{
    std::cerr << err << ": " << dbus_error.message << "\n";
    dbus_error_free(&dbus_error);
}

int main(int argc, char** argv)
{
    DBusConnection* conn;
    int ret;
    // char input[80];
    constexpr uint INPUTSIZE = 80;
    std::vector<char> input(INPUTSIZE);

    dbus_error_init(&dbus_error);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);

    if (dbus_error_is_set(&dbus_error))
        print_dbus_error("dbus_bus_get");

    if (!conn)
        exit(1);


    printf("Please type two numbers: ");
    while (std::fgets (input.data(), INPUTSIZE, stdin) != nullptr) {

        // Get a well known name
        while (1) {
            ret = dbus_bus_request_name(conn, CLIENT_BUS_NAME, 0, &dbus_error);

            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
                break;

            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
                fprintf(stderr, "Waiting for the bus ... \n");
                sleep(1);
                continue;
            }
            if (dbus_error_is_set(&dbus_error))
                print_dbus_error("dbus_bus_get");
        }

        DBusMessage* request;

        if ((request = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME,
            INTERFACE_NAME, METHOD_NAME)) == NULL) {
            std::cout << "Error in dbus_message_new_method_call\n";
            return 1;
        }

        DBusMessageIter iter;
        dbus_message_iter_init_append(request, &iter);
        char* ptr = input.data();
        if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &ptr)) {
            std::cerr << "Error in dbus_message_iter_append_basic\n";
            return 1;
        }
        DBusPendingCall* pending_return;
        if (!dbus_connection_send_with_reply(conn, request, &pending_return, -1)) {
            std::cerr << "Error in dbus_connection_send_with_reply\n";
            return 1;
        }

        if (pending_return == NULL) {
            std::cerr << "pending return is NULL\n";
            return 1;
        }

        dbus_connection_flush(conn);

        dbus_message_unref(request);

        dbus_pending_call_block(pending_return);

        DBusMessage* reply;
        if ((reply = dbus_pending_call_steal_reply(pending_return)) == NULL) {
            std::cerr << "Error in dbus_pending_call_steal_reply\n";
            return 1;
        }

        dbus_pending_call_unref(pending_return);

        char* s;
        if (dbus_message_get_args(reply, &dbus_error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
            std::cout << s << "\n";
        }
        else
        {
            std::cerr << "Did not get arguments in reply\n";
            return 1;
        }
        dbus_message_unref(reply);

        if (dbus_bus_release_name(conn, CLIENT_BUS_NAME, &dbus_error) == -1) {
            std::cerr << "Error in dbus_bus_release_name\n";
            return 1;
        }
        std::cout << "Please type two numbers: ";
    }

    return 0;
}