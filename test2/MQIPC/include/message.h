#pragma once
#include <string>
#include <iostream>
#include <cstring>
#include <memory>
#include <mutex>
#include <variant>
#include <vector>

namespace MQIPC
{
    using ArgVariant = std::variant<int, double, std::string>;

    struct Message {
        enum Type
        {
            CONNECT,
            REGISTRY_SERVICE,
            SUBSCRIBE,
            UNSUBSCRIBE,
            METHOD_CALL,
            ASYNC_CALL,
            METHOD_RETURN,
            BROADCAST_SIGNAL,
            SUBSCRIBE_SIGNAL,
            RESPONSE,
            DISCONNECT,
            TERMINATE
        };

        enum ArgType
        {
            NONE,
            INTEGER,
            DOUBLE,
            STRING
        };

        Type type{};
        char from[32]{};
        char to[32]{};
        char content[32]{};
        char correlation_id[8]{};

        uint8_t argc;
        
        static constexpr size_t MAX_ARGS{5};

        ArgType arg_types[MAX_ARGS];

        struct Argument
        {
            union
            {
                int int_val;
                double double_val;
                char string_val[24];
            } value;
        } args[MAX_ARGS];
    };

    inline constexpr size_t MSG_SZ = sizeof(MQIPC::Message);
    //Helper classes
    class MessageBuilder
    {
    private:
        std::unique_ptr<MQIPC::Message> _message;
        void set_arguments(uint8_t arg_index, const MQIPC::ArgVariant& argv);
    public:
        MessageBuilder();
        MessageBuilder(const MQIPC::Message& message);
        void reset();
        void reset(const MQIPC::Message& message);

        // Delete copy constructor and assignment
        MessageBuilder(const MessageBuilder&) = delete;
        MessageBuilder& operator=(const MessageBuilder&) = delete;

        // Add move constructor and assignment
        MessageBuilder(MessageBuilder&&) = default;
        MessageBuilder& operator=(MessageBuilder&&) = default;

        MQIPC::MessageBuilder& add_type(MQIPC::Message::Type msg_type);
        MQIPC::MessageBuilder& add_sender(const char* msg_sender);
        MQIPC::MessageBuilder& add_receiver(const char* msg_receiver);
        MQIPC::MessageBuilder& add_content(const char* msg_content);
        MQIPC::MessageBuilder& add_correlation_id(const char* id);
        MQIPC::MessageBuilder& add_args(const std::vector<MQIPC::ArgVariant>& arg_vec);
        std::unique_ptr<MQIPC::Message> message();
    };

    //Debug functions

    std::ostream& operator<<(std::ostream& out, const MQIPC::Message::Type value);
    std::ostream& operator<<(std::ostream& out, const MQIPC::Message msg);
}