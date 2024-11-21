#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

// Kiểu dữ liệu của sự kiện
using Event = std::function<void()>;

// Lớp mô phỏng Event Loop
class EventLoop {
public:
    // Hàm thêm sự kiện vào hàng đợi
    void addEvent(Event event) {
        std::unique_lock<std::mutex> lock(mutex_);
        events.push(event);
        cv_.notify_one();  // Báo hiệu có sự kiện mới
    }

    // Hàm bắt đầu vòng lặp sự kiện
    void run() {
        while (true) {
            Event event;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                // Đợi cho đến khi có sự kiện mới
                cv_.wait(lock, [this]() { return !events.empty();}); //tiếp tục chỉ khi queue có event
                // Lấy sự kiện ra khỏi hàng đợi
                event = events.front();
                events.pop();
            }
            
            // Xử lý sự kiện
            event();
            
            // Đợi một thời gian ngắn trước khi kiểm tra lại
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

private:
    std::queue<Event> events;       // Hàng đợi chứa các sự kiện
    std::mutex mutex_;              // Bảo vệ hàng đợi khi truy cập đa luồng
    std::condition_variable cv_;    // Biến điều kiện để thông báo có sự kiện mới
};

int main() {
    EventLoop loop;

    // Tạo một luồng chạy Event Loop
    std::thread loopThread([&loop]() {
        std::cout << "Starting Event Loop..." << std::endl;
        loop.run();
    });

    // Thêm các sự kiện vào Event Loop ở các thời điểm khác nhau
    std::this_thread::sleep_for(std::chrono::seconds(1));
    loop.addEvent([]() {
        std::cout << "Handling event 1" << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    loop.addEvent([]() {
        std::cout << "Handling event 2" << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
    loop.addEvent([]() {
        std::cout << "Handling event 3" << std::endl;
    });

    // Cho phép thời gian để các sự kiện được xử lý trước khi thoát chương trình
    loopThread.join();
    return 0;
}
