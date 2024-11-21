#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <cstdio>
#include <iostream>
 
void signal_handler(int signum) {
    // Ghi thông báo lỗi trực tiếp ra stderr (stdout cũng được chấp nhận nếu cần)
    const char *message = "Received signal, writing stack trace...\n";
    write(STDERR_FILENO, message, sizeof(message) - 1);
 
    // Mở file an toàn với tín hiệu bằng hàm open thay vì fopen
    int log_fd = open("crash.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_fd == -1) {
        const char *error_msg = "Failed to open log file\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);
        _exit(1);  // Thoát ngay lập tức mà không gọi hàm không an toàn
    }
 
    // Ghi lại backtrace
    void *array[10];
    size_t size = backtrace(array, 10);
 
    // Ghi thông báo tín hiệu vào file
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "Error: signal %d\n", signum);
    write(log_fd, buffer, len);  // Dùng write để ghi trực tiếp
 
    // Ghi lại stack trace vào file
    backtrace_symbols_fd(array, size, log_fd);  // Ghi trực tiếp vào file descriptor
 
    // Đóng file log
    close(log_fd);
 
    // Thoát ngay lập tức mà không gọi exit() để tránh việc cleanup không an toàn
    _exit(1);
}
 
void function1(){
    // Gây ra lỗi segmentation fault
    int *ptr = NULL;
    *ptr = 42;  // Lỗi truy cập bộ nhớ không hợp lệ
}
 
void function2(){
    int a = 3;
    std::cout << a << std::endl;
    function1();
}
 
int main() {
    // Đăng ký signal handler
    signal(SIGSEGV, signal_handler);
    // signal(SIGABRT, signal_handler);
    function2();
   
 
    return 0;
}