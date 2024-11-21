// server.cpp
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/my_shared_memory" // POSIX shared memory name
#define SHM_SIZE 1024                // Size of shared memory

struct SharedData {
    int flag;              // 0 = server writing, 1 = client reading
    char message[100];     // Shared message buffer
};

int main() {
    // Create shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory\n";
        return 1;
    }

    // Set the size of shared memory
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        std::cerr << "Failed to set shared memory size\n";
        return 1;
    }

    // Map shared memory in the address space
    // SharedData* sharedData = static_cast<SharedData*>(mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    void* someAddress = malloc(sizeof(SharedData));
    SharedData* sharedData = static_cast<SharedData*>(mmap(&someAddress, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    if (sharedData == MAP_FAILED) {
        std::cerr << "Failed to map shared memory\n";
        return 1;
    }

    // Initialize shared memory
    sharedData->flag = 0;
    std::string input;

    while (true) {
        // Wait until the client has read the previous message
        while (sharedData->flag == 1) {
            usleep(100); // Small delay to avoid busy-waiting
        }
        // Get message input from the user
        std::cout << "Enter message: ";
        std::getline(std::cin, input);

        // Copy message to shared memory
        strncpy(sharedData->message, input.c_str(), sizeof(sharedData->message) - 1);
        sharedData->message[sizeof(sharedData->message) - 1] = '\0'; // Null-terminate
    
        sharedData->flag = 1; // Notify client to read

        // Exit if the message is "end"
        if (input == "end") {
            break;
        }
    }
    // Clean up
    munmap(sharedData, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    close(shm_fd);
    return 0;
}
