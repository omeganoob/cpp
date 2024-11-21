#include <mqueue.h>
#include <string.h>
#include <iostream>
#include <vector>

// 1. Fixed message structure approach
struct FixedMessage {
    int type;
    char sender[32];
    char method[32];
    double args[2];
};

// 2. Raw struct approach (be careful with padding/alignment!)
#pragma pack(push, 1)  // Force 1-byte alignment
struct PackedMessage {
    int type;
    double value;
    char text[20];
};
#pragma pack(pop)

class MessageQueueDemo {
public:
    static void demoFixedStruct() {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(FixedMessage);
        attr.mq_curmsgs = 0;

        // Create queue
        mqd_t queue = mq_open("/demo_queue", O_CREAT | O_RDWR, 0644, &attr);
        
        // Send message
        FixedMessage msg;
        msg.type = 1;
        strncpy(msg.sender, "client1", sizeof(msg.sender));
        strncpy(msg.method, "add", sizeof(msg.method));
        msg.args[0] = 5.0;
        msg.args[1] = 3.0;
        
        mq_send(queue, (char*)&msg, sizeof(msg), 0);
        
        // Receive message
        FixedMessage received;
        mq_receive(queue, (char*)&received, sizeof(FixedMessage), nullptr);
        
        std::cout << "Fixed struct received: " << received.sender 
                  << " " << received.method << " " 
                  << received.args[0] << " " << received.args[1] << std::endl;
                  
        mq_close(queue);
        mq_unlink("/demo_queue");
    }

    static void demoPackedStruct() {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(PackedMessage);
        attr.mq_curmsgs = 0;

        mqd_t queue = mq_open("/packed_queue", O_CREAT | O_RDWR, 0644, &attr);
        
        // Send packed message
        PackedMessage msg{1, 3.14, "Hello"};
        mq_send(queue, (char*)&msg, sizeof(msg), 0);
        
        // Receive packed message
        PackedMessage received;
        mq_receive(queue, (char*)&received, sizeof(PackedMessage), nullptr);
        
        std::cout << "Packed struct received: " << received.type 
                  << " " << received.value << " " << received.text << std::endl;
                  
        mq_close(queue);
        mq_unlink("/packed_queue");
    }

    // 3. Variable-length message approach using serialization
    static void demoVariableLength() {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = 8192;  // Larger buffer for variable-length data
        attr.mq_curmsgs = 0;

        mqd_t queue = mq_open("/var_queue", O_CREAT | O_RDWR, 0644, &attr);
        
        // Create message buffer
        struct VarMessage {
            int type;
            std::string text;
            std::vector<double> values;
        } msg;
        
        msg.type = 1;
        msg.text = "Example";
        msg.values = {1.1, 2.2, 3.3, 4.4};
        
        // Serialize (simple format: type|textlength|text|valueslength|values)
        std::string buffer;
        buffer.reserve(8192);
        
        // Add type
        buffer.append((char*)&msg.type, sizeof(int));
        
        // Add text
        int textLen = msg.text.length();
        buffer.append((char*)&textLen, sizeof(int));
        buffer.append(msg.text);
        
        // Add values
        int valuesLen = msg.values.size();
        buffer.append((char*)&valuesLen, sizeof(int));
        buffer.append((char*)msg.values.data(), valuesLen * sizeof(double));
        
        // Send
        mq_send(queue, buffer.c_str(), buffer.length(), 0);
        
        // Receive and deserialize
        char recv_buffer[8192];
        ssize_t bytes = mq_receive(queue, recv_buffer, 8192, nullptr);
        
        if(bytes > 0) {
            size_t offset = 0;
            
            // Read type
            int recv_type;
            memcpy(&recv_type, recv_buffer + offset, sizeof(int));
            offset += sizeof(int);
            
            // Read text
            int recv_textLen;
            memcpy(&recv_textLen, recv_buffer + offset, sizeof(int));
            offset += sizeof(int);
            
            std::string recv_text(recv_buffer + offset, recv_textLen);
            offset += recv_textLen;
            
            // Read values
            int recv_valuesLen;
            memcpy(&recv_valuesLen, recv_buffer + offset, sizeof(int));
            offset += sizeof(int);
            
            std::vector<double> recv_values(recv_valuesLen);
            memcpy(recv_values.data(), recv_buffer + offset, recv_valuesLen * sizeof(double));
            
            std::cout << "Variable length received: Type=" << recv_type 
                      << " Text=" << recv_text << " Values=";
            for(double v : recv_values) std::cout << v << " ";
            std::cout << std::endl;
        }
        
        mq_close(queue);
        mq_unlink("/var_queue");
    }
};

int main() {
    std::cout << "1. Fixed struct demo:\n";
    MessageQueueDemo::demoFixedStruct();
    
    std::cout << "\n2. Packed struct demo:\n";
    MessageQueueDemo::demoPackedStruct();
    
    std::cout << "\n3. Variable length demo:\n";
    MessageQueueDemo::demoVariableLength();
    
    return 0;
}