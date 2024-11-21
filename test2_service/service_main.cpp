#include <iostream>
#include <thread>
#include <chrono>
#include <ipc_app.h>

int main(int argc, char** argv) {
    MQIPC::App service(argv[1], "my_server");
    if (!service.connect()) {
        return 1;
    }

    if (!service.register_service("test_method", 5'000) ||
        !service.register_service("test_method2", 5'000)) {
        std::cout << "Cannot register services.\n";
        return 1;
    }

    // Start message handling in background
    service.dispatch_message([&](MQIPC::Message msg) {
        switch (msg.type) {
        case MQIPC::Message::Type::METHOD_CALL: {
            std::string method(msg.content);
            std::cout << "Someone called " << method << "\n";
            if (method == "test_method2") {
                service.method_return(msg.from, msg, { "got chu!" });
            }
            break;
        }
        case MQIPC::Message::Type::ASYNC_CALL: {
            std::string method(msg.content);
            std::cout << "Someone called(async) " << method << "\n";
            break;
        }
        case MQIPC::Message::Type::DISCONNECT: {
            std::cout << "MQIPC server down!\n";
            break;
        }
        default:
            break;
        }
        });

    // Main program loop
    std::string command;
    while (true) {
        std::cout << "\nEnter command (signal/quit): ";
        std::getline(std::cin, command);

        if (command == "quit") {
            break;
        }
        else if (command == "signal") {
            service.send_signal(true, "test_signal", { "Hello from signal!" });
        }
        else if (command == "signal2")
        {
            
        }
    }
    return 0;
}