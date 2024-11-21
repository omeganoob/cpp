#include "ipc_process.h"
#include <iostream>
#include <string>
#include <sstream>

void service_process()
{
    // Nhập tên server và service
    std::string server_name;
    std::string service_name;
    std::cout << "Enter server name: ";
    std::getline(std::cin, server_name);
    std::cout << "Enter service name: ";
    std::getline(std::cin, service_name);

    // Khởi tạo và kết nối service
    KIPC::Client service(service_name, server_name);
    if (!service.connect())
    {
        std::cerr << "Failed to connect to server.\n";
        return;
    }

    std::string shared_data = "25.0"; // Giá trị nhiệt độ mặc định

    // Đăng ký các method
    if (!service.register_service(service_name, "get_temp"))
    {
        std::cerr << "Failed to register method: get_temp\n";
        return;
    }
    if (!service.register_service(service_name, "set_temp"))
    {
        std::cerr << "Failed to register method: set_temp\n";
        return;
    }

    std::cout << "Service registered successfully. Waiting for client requests...\n";

    while (true)
    {
        // Xử lý tin nhắn đến
        if (!service.loop())
        {
            std::cerr << "Error in receiving messages.\n";
        }

        // Kiểm tra và xử lý các yêu cầu method call
        KIPC::Message request;
        while (service.receive_request(request))
        {
            std::string response;

            // Xử lý method call
            if (request.method_name == "get_temp")
            {
                // Trả về nhiệt độ hiện tại
                response = shared_data;
            }
            else if (request.method_name == "set_temp")
            {
                // Cập nhật nhiệt độ từ đối số
                std::string new_temp = request.arguments;
                shared_data = new_temp;
                response = "Temperature set to " + new_temp;
            }
            else
            {
                response = "Unknown method: " + request.method_name;
            }

            // Gửi phản hồi lại client
            if (!service.send_response(request.sender, response))
            {
                std::cerr << "Failed to send response to client.\n";
            }
        }
    }

    // Ngắt kết nối trước khi thoát
    service.disconnect();
}

int main()
{
    service_process();
    return 0;
}

// #include "ipc_process.h"
// #include <iostream>
// #include <string>

// void service_process()
// {
//     // Nhập tên server và service
//     std::string server_name;
//     std::string service_name;
//     std::cout << "Enter server name: ";
//     std::getline(std::cin, server_name);
//     std::cout << "Enter service name: ";
//     std::getline(std::cin, service_name);

//     // Khởi tạo và kết nối service
//     KIPC::Client service(service_name, server_name);
//     if (!service.connect())
//     {
//         std::cerr << "Failed to connect to server.\n";
//         return;
//     }

//     std::string action;
//     std::string shared_data = "Initial Data"; // Dữ liệu dùng chung

//     while (true)
//     {
//         // Hiển thị menu hành động
//         std::cout << "\nChoose action:\n";
//         std::cout << "1. Register service method\n";
//         std::cout << "2. Send signal to subscribers\n";
//         std::cout << "3. Set shared data\n";
//         std::cout << "4. Get shared data\n";
//         std::cout << "5. Exit\n";
//         std::cout << "Enter your choice: ";
//         std::getline(std::cin, action);

//         if (action == "1")
//         {
//             // Đăng ký phương thức của service
//             std::string method_name;
//             std::cout << "Enter method name to register (e.g., SetData/GetData): ";
//             std::getline(std::cin, method_name);

//             if (!service.register_service(service_name, method_name))
//             {
//                 std::cerr << "Failed to register method: " << method_name << "\n";
//             }
//             else
//             {
//                 std::cout << "Method " << method_name << " registered successfully.\n";
//                 continue;
//             }
//         }
//         else if (action == "2")
//         {
//             // Gửi tín hiệu đến các client đã đăng ký
//             std::string signal_message;
//             std::cout << "Enter signal message to send: ";
//             std::getline(std::cin, signal_message);

//             if (!service.send_signal(service_name, signal_message))
//             {
//                 std::cerr << "Failed to send signal: " << signal_message << "\n";
//             }
//             else
//             {
//                 std::cout << "Signal sent: " << signal_message << "\n";
//                 continue;
//             }
//         }
//         else if (action == "3")
//         {
//             // Cập nhật dữ liệu dùng chung
//             std::cout << "Enter new shared data: ";
//             std::getline(std::cin, shared_data);

//             // Gửi tín hiệu thông báo cập nhật
//             if (!service.send_signal(service_name, "DataChanged"))
//             {
//                 std::cerr << "Failed to notify subscribers about data change.\n";
//             }
//             else
//             {
//                 std::cout << "Shared data updated: " << shared_data << "\n";
//             }
//         }
//         else if (action == "4")
//         {
//             // Hiển thị dữ liệu hiện tại
//             std::cout << "Current shared data: " << shared_data << "\n";
//         }
//         else if (action == "5")
//         {
//             // Thoát chương trình
//             std::cout << "Exiting service...\n";
//             break;
//         }
//         else
//         {
//             std::cerr << "Invalid action. Please try again.\n";
//         }

//         // Xử lý tin nhắn đến
//         if (!service.loop())
//         {
//             std::cerr << "Error in receiving messages.\n";
//         }
//     }

//     // Ngắt kết nối trước khi thoát
//     service.disconnect();
// }

// int main()
// {
//     service_process();
//     return 0;
// }
