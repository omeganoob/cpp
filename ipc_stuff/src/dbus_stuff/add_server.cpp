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
constexpr char* OBJECT_PATH_NAME = "/chung/dep/zai/adder";
constexpr char* METHOD_NAME = "add_numbers";

// Initialize DBus error handling
DBusError dbus_error;
void print_dbus_err(std::string_view err)
{
    std::cerr << err << ": " << dbus_error.message << "\n";
    dbus_error_free(&dbus_error);
}
bool is_integer(char* ptr)
{
    if (*ptr == '+' || *ptr == '-')
    {
        ptr++;
    }

    while (*ptr)
    {
        if (!isdigit((int)*ptr++))
        {
            return false;
        }
    }

    return true;
}

DBusHandlerResult message_handler(DBusConnection* connection, DBusMessage* message, void* user_data);

DBusObjectPathVTable vtable = {
    .unregister_function = NULL,
    .message_function = message_handler
};

int main() {
    dbus_error_init(&dbus_error);

    // Connect to the session bus
    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);

    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_err("dbus_bus_get");
        dbus_error_free(&dbus_error);
    }

    if (!conn) {
        std::cerr << "Failed to connect to the D-Bus session bus." << std::endl;
        return 1;
    }

    std::cout << "Connected to the D-Bus session bus successfully!" << std::endl;

    int return_stt = dbus_bus_request_name(conn, SERVER_BUS_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);

    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_err("dbus_bus_request_name");
        dbus_error_free(&dbus_error);
    }

    if (return_stt != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        std::cerr << "Dbus: not primary owner, return_stt = " << return_stt << "\n";
        return 1;
    }

    if (!dbus_connection_register_object_path(conn, OBJECT_PATH_NAME, &vtable, NULL)) {
        std::cerr << "Failed to register object path: " << OBJECT_PATH_NAME << std::endl;
        return 1;
    }

    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_err("dbus_connection_try_register_object_path");
        dbus_error_free(&dbus_error);
    }

    dbus_connection_add_filter(conn, message_handler, NULL, NULL);

    //Handler client request
    while (true)
    {
        // Block for message from client
        if (!dbus_connection_read_write_dispatch(conn, -1))
        {
            std::cerr << "Not connected now\n";
            return 1;
        }
        usleep(300);
    }

    // Cleanup and close connection
    dbus_connection_unref(conn);
    return 0;
}

DBusHandlerResult message_handler(DBusConnection* conn, DBusMessage* msg, void* user_data)
{
    if (dbus_message_is_method_call(msg, INTERFACE_NAME, METHOD_NAME))
    {
        char* s;
        char* str1 = nullptr;
        char* str2 = nullptr;
        const char space[4] = " \n\t";
        long i, j;
        bool error = false;

        if (dbus_message_get_args(msg, &dbus_error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID))
        {
            std::cout << s;
            //Validate message
            str1 = strtok(s, space);
            if (str1)
            {
                str2 = strtok(nullptr, space);
            }

            if (!str1 || !str2)
            {
                error = true;
            }

            if (!error)
            {
                if (is_integer(str1))
                {
                    i = atol(str1);
                }
                else
                {
                    error = true;
                }
            }

            if (!error)
            {
                if (is_integer(str2))
                {
                    j = atol(str2);
                }
                else
                {
                    error = true;
                }
            }

            if (!error)
            {
                //send reply
                DBusMessage* reply;
                std::vector<char> answer(40);
                std::cout << "Sum is " << i + j << "\n";
                std::snprintf(answer.data(), answer.size(), "Sum is %ld", i + j);
                if ((reply = dbus_message_new_method_return(msg)) == nullptr)
                {
                    std::cerr << "Error in dbus_message_new_method_return\n";
                    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }

                DBusMessageIter iter;
                dbus_message_iter_init_append(reply, &iter);
                char* ptr = answer.data();
                if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &ptr))
                {
                    std::cerr << "dbus_message_iter_append_basic\n";
                    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }

                if (!dbus_connection_send(conn, reply, nullptr))
                {
                    std::cerr << "dbus_connection_send\n";
                    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }

                dbus_connection_flush(conn);
                dbus_message_unref(reply);
            }
            else
            {
                //handle error
                DBusMessage* dbus_error_msg;
                std::string error_msg = "Error in input.";
                if ((dbus_error_msg = dbus_message_new_error(msg, DBUS_ERROR_FAILED, error_msg.c_str())) == nullptr)
                {
                    std::cerr << "dbus_message_new_error\n";
                    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }

                if (!dbus_connection_send(conn, dbus_error_msg, nullptr))
                {
                    std::cerr << "dbus_connection_send\n";
                    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }

                dbus_connection_flush(conn);
                dbus_message_unref(dbus_error_msg);
            }
        }
        else
        {
            print_dbus_err("Error getting message");
        }
    }
    return DBUS_HANDLER_RESULT_HANDLED;
}
