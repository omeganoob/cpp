#include "server_queue.h"
#include <iostream>
#include <thread>

int main()
{
    try
    {
        // Khởi tạo server
        KIPC::Server server("server_test");

        // Chạy server trong một luồng riêng
        std::thread server_thread([&server]()
                                  { server.start(); });

        std::cout << "Press Enter to stop the server..." << std::endl;
        std::cin.get();

        // Dừng server
        server.stop();
        server_thread.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
