#include <fcntl.h>    // For open()
#include <stdio.h>
#include <unistd.h>   // For read(), write(), close()
#include <sys/stat.h>
#include <iostream>

int main() {
    int file = open("../example.txt", O_CREAT, O_WRONLY);  // Open the file in read-only mode
    if (file < 0) {
        perror("Error opening file");
        return 1;
    }

    std::cout << file << "\n";

    char w_buffer[128] = "nguyen quang chung";
    write(file, w_buffer, sizeof(w_buffer) - 1);

    char buffer[128];
    ssize_t bytesRead = read(file, buffer, sizeof(buffer) - 1);  // Read file contents
    if (bytesRead < 0) {
        perror("Error reading file");
        close(file);
        return 1;
    }

    buffer[bytesRead] = '\0';  // Null-terminate the string
    printf("File contents: %s\n", buffer);

    close(file);  // Close the file

    // if (mkdir("../testpath", 0777) < 0) {
    //     perror("Error creating directory");
    //     return 1;
    // }

    return 0;
}
