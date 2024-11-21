#include <mqueue.h>
#include <string.h>
#include <iostream>
#include <variant>
#include <array>

// Approach 1: Using union
struct FixedMessageUnion {
    int type;
    char sender[32];
    char method[32];
    union {
        struct {
            double num1;
            double num2;
        } numbers;
        struct {
            char str1[64];
            char str2[64];
        } strings;
        struct {
            double num;
            char str[64];
        } mixed;
    } args;
};

// Approach 2: Using std::variant and fixed-size arrays
struct FixedMessageVariant {
    int type;
    char sender[32];
    char method[32];
    // Define possible argument types
    using ArgType = std::variant<double, char[64]>;
    std::array<ArgType, 2> args;  // Fixed size array of variants
};

class MessageQueueDemo {
public:
    static void demoUnionMessage() {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(FixedMessageUnion);
        attr.mq_curmsgs = 0;

        mqd_t queue = mq_open("/demo_queue", O_CREAT | O_RDWR, 0644, &attr);
        
        // Example 1: Sending numbers
        FixedMessageUnion msg1;
        msg1.type = 1;  // Type 1 for numbers
        strncpy(msg1.sender, "client1", sizeof(msg1.sender));
        strncpy(msg1.method, "add", sizeof(msg1.method));
        msg1.args.numbers.num1 = 5.0;
        msg1.args.numbers.num2 = 3.0;
        
        mq_send(queue, (char*)&msg1, sizeof(msg1), 0);
        
        // Example 2: Sending strings
        FixedMessageUnion msg2;
        msg2.type = 2;  // Type 2 for strings
        strncpy(msg2.sender, "client1", sizeof(msg2.sender));
        strncpy(msg2.method, "concat", sizeof(msg2.method));
        strncpy(msg2.args.strings.str1, "Hello", sizeof(msg2.args.strings.str1));
        strncpy(msg2.args.strings.str2, "World", sizeof(msg2.args.strings.str2));
        
        mq_send(queue, (char*)&msg2, sizeof(msg2), 0);

        // Receiving and processing messages
        FixedMessageUnion received;
        mq_receive(queue, (char*)&received, sizeof(FixedMessageUnion), nullptr);
        
        // Process based on type
        switch(received.type) {
            case 1:  // Numbers
                std::cout << "Number arguments: " 
                         << received.args.numbers.num1 << ", "
                         << received.args.numbers.num2 << std::endl;
                break;
            case 2:  // Strings
                std::cout << "String arguments: " 
                         << received.args.strings.str1 << ", "
                         << received.args.strings.str2 << std::endl;
                break;
        }
        
        mq_close(queue);
        mq_unlink("/demo_queue");
    }

    // Helper function to process variant messages
    static void processVariantMessage(const FixedMessageVariant& msg) {
        std::cout << "Sender: " << msg.sender << ", Method: " << msg.method << "\n";
        for (size_t i = 0; i < msg.args.size(); i++) {
            std::cout << "Arg " << i << ": ";
            if (auto* d = std::get_if<double>(&msg.args[i])) {
                std::cout << "Number: " << *d;
            } else {
                // Handle char array variant
                std::cout << "String: " << std::get<char[64]>(msg.args[i]);
            }
            std::cout << "\n";
        }
    }
};

int main() {
    std::cout << "Union-based message demo:\n";
    MessageQueueDemo::demoUnionMessage();
    return 0;
}