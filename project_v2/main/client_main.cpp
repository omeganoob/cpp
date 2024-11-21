#include "ipc_process.h"
#include <iostream>
#include <string>

void client_process()
{
    // Nhập tên process
    std::string client_name;
    std::string server_name;
    std::cout << "Enter client server name: ";
    std::getline(std::cin, server_name);
    std::cout << "Enter client process name: ";
    std::getline(std::cin, client_name);

    // Khởi tạo và kết nối client
    KIPC::Client client(client_name, server_name);
    if (!client.connect())
    {
        std::cerr << "Failed to connect to server.\n";
        return;
    }

    std::string action;
    while (true)
    {
        // Hiển thị menu hành động
        std::cout << "\nChoose action:\n";
        std::cout << "1. GET data\n";
        std::cout << "2. SET data\n";
        std::cout << "3. SUBSCRIBE to service\n";
        std::cout << "4. UNSUBSCRIBE from service\n";
        std::cout << "Enter your choice: ";
        std::getline(std::cin, action);

        if (action == "1")
        {
            // Gửi yêu cầu GET dữ liệu
            if (!client.call_method("service1", "get_temp", ""))
            {
                std::cerr << "Failed to send GET request.\n";
            }
            continue;
        }
        else if (action == "2")
        {
            // Gửi yêu cầu SET dữ liệu
            std::string temp;
            std::cout << "Enter temperature to set: ";
            std::getline(std::cin, temp);
            if (!client.call_method("service1", "set_temp", temp))
            {
                std::cerr << "Failed to send SET request.\n";
            }
        }
        else if (action == "3")
        {
            // Đăng ký SUBSCRIBE tới service
            std::string service_name;
            std::cout << "Enter service name to subscribe: ";
            std::getline(std::cin, service_name);
            client.subscribe_service(service_name);

            continue;
        }
        else if (action == "4")
        {
            // Hủy đăng ký SUBSCRIBE từ service
            std::string service_name;
            std::cout << "Enter service name to unsubscribe: ";
            std::getline(std::cin, service_name);
            client.unsubscribe_service(service_name);
            continue;
        }
        else
        {
            std::cout << "Invalid action. Please try again.\n";
        }

        // Nhận thông báo từ server (nếu có)
        if (!client.loop())
        {
            std::cerr << "Error in receiving messages.\n";
        }
    }
}

int main()
{
    client_process();
    return 0;
}
