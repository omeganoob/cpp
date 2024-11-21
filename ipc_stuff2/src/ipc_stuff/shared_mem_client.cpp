// client.cpp
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define SHM_NAME "/my_shared_memory" // POSIX shared memory name
#define SHM_SIZE 1024                // Size of shared memory

struct SharedData {
    int flag;              // 0 = server writing, 1 = client reading
    char message[100];     // Shared message buffer
};

int main() {
    // Open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory\n";
        return 1;
    }

    // Map shared memory in the address space
    SharedData* sharedData = static_cast<SharedData*>(mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (sharedData == MAP_FAILED) {
        std::cerr << "Failed to map shared memory\n";
        return 1;
    }

    while (true) {
        // Wait until the server has written a message
        while (sharedData->flag == 0) {
            usleep(100); // Small delay to avoid busy-waiting
        }

        // Print the received message
        std::cout << "Received message: " << sharedData->message << "\n";

        // Check if the message is "end" to terminate
        if (strcmp(sharedData->message, "end") == 0) {
            break;
        }

        // Set flag back to 0 to signal the server to write again
        sharedData->flag = 0;
    }

    // Clean up
    munmap(sharedData, sizeof(SharedData));
    close(shm_fd);
    return 0;
}
