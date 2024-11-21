#include <cstring>
#include "communication.h"
#include "server_queue.h"

void KIPC::printMessage(const Message &msg)
{
    std::cout << "Message type: " << static_cast<int>(msg.type) << "\n";
    std::cout << "From: " << msg.sender << "\n";
    std::cout << "To: " << msg.receiver << "\n";
    std::cout << "Content: " << msg.parameters << "\n";
};
