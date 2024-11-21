#include <iostream>
#include <ipc_app.h>

void message_handler(MQIPC::Message msg)
{
    switch (msg.type)
    {
    case MQIPC::Message::Type::METHOD_RETURN:
    {
        std::cout << "get return\n";
        std::string method(msg.content);
        if (method == "test_method2")
        {
            std::cout << "received response from service1.test_method2\n";
            for (int i = 0; i < msg.argc; i++) {
                std::cout << "Arg " << i << ": ";
                switch (msg.arg_types[i]) {
                case MQIPC::Message::ArgType::INTEGER:
                    std::cout << msg.args[i].value.int_val;
                    break;
                case MQIPC::Message::ArgType::DOUBLE:
                    std::cout << msg.args[i].value.double_val;
                    break;
                case MQIPC::Message::ArgType::STRING:
                    std::cout << msg.args[i].value.string_val;
                    break;
                default:
                    break;
                }
                std::cout << "\n";
            }
        }
        break;
    }
    case MQIPC::Message::Type::BROADCAST_SIGNAL:
    {
        std::cout << "Catch signal: \n";
        std::cout << msg;
        break;
    }
    case MQIPC::Message::Type::DISCONNECT:
    {
        std::cout << "MQIPC server down!\n";
        break;
    }
    default:
        break;
    }
}

int main(int, char** argv) {
    std::cout << MQIPC::MSG_SZ << "\n";
    MQIPC::App client(argv[1], "my_server");
    if (!client.connect())
    {
        return 1;
    }
    client.dispatch_message(message_handler);
    client.call_method("service1", "test_method", {}, true);
    client.call_method("service1", "test_method2", { 111, 222, "chung" });

    // Main program loop
    std::string command;
    while (true) {
        std::cout << "\nEnter command (signal/quit): ";
        std::getline(std::cin, command);
        if (command == "quit") {
            break;
        }
        if (command == "1")
        {
            client.call_method("service1", "test_method", {}, true);
        }
        if (command == "2")
        {
            client.call_method("service1", "test_method2", { 111, 222, "chung" });
        }
    }
    return 0;
}

