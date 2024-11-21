#include "message.h"

std::ostream& MQIPC::operator<<(std::ostream& out, const MQIPC::Message::Type value) {
    return out << [value] {
#define PROCESS_VAL(p) case(p): return #p;
        switch (value) {
            PROCESS_VAL(MQIPC::Message::Type::CONNECT);
            PROCESS_VAL(MQIPC::Message::Type::REGISTRY_SERVICE);
            PROCESS_VAL(MQIPC::Message::Type::SUBSCRIBE);
            PROCESS_VAL(MQIPC::Message::Type::UNSUBSCRIBE);
            PROCESS_VAL(MQIPC::Message::Type::METHOD_CALL);
            PROCESS_VAL(MQIPC::Message::Type::ASYNC_CALL);
            PROCESS_VAL(MQIPC::Message::Type::METHOD_RETURN);
            PROCESS_VAL(MQIPC::Message::Type::BROADCAST_SIGNAL);
            PROCESS_VAL(MQIPC::Message::Type::SUBSCRIBE_SIGNAL);
            PROCESS_VAL(MQIPC::Message::Type::RESPONSE);
            PROCESS_VAL(MQIPC::Message::Type::DISCONNECT);
            PROCESS_VAL(MQIPC::Message::Type::TERMINATE);
        default: return "UNKNOWN";
        }
#undef PROCESS_VAL
        }();
}

std::ostream& MQIPC::operator<<(std::ostream& out, const MQIPC::Message msg)
{
    out << "Type: " << msg.type << "\n"
        << "From: " << msg.from << "\n"
        << "To: " << msg.to << "\n"
        << "Content: " << msg.content << "\n"
        << "Correlation ID: " << msg.correlation_id << "\n"
        << "Arguments: ";
    for (uint8_t i = 0; i < msg.argc; ++i) {
        switch (msg.arg_types[i]) {
        case MQIPC::Message::INTEGER:
            out << msg.args[i].value.int_val << " ";
            break;
        case MQIPC::Message::DOUBLE:
            out << msg.args[i].value.double_val << " ";
            break;
        case MQIPC::Message::STRING:
            out << msg.args[i].value.string_val << " ";
            break;
        default:
            out << "UNKNOWN ";
        }
    }
    out << std::endl;
    return out;
}

void MQIPC::MessageBuilder::set_arguments(uint8_t arg_index, const MQIPC::ArgVariant& argv)
{
    if (arg_index >= MQIPC::Message::MAX_ARGS)
    {
        throw std::runtime_error("Message argument index out of bounds!");
    }

    if (std::holds_alternative<int>(argv))
    {
        _message->arg_types[arg_index] = MQIPC::Message::ArgType::INTEGER;
        _message->args[arg_index].value.int_val = std::get<int>(argv);
        return;
    }
    else if (std::holds_alternative<double>(argv))
    {
        _message->arg_types[arg_index] = MQIPC::Message::ArgType::DOUBLE;
        _message->args[arg_index].value.double_val = std::get<double>(argv);

        return;
    }
    else if (std::holds_alternative<std::string>(argv))
    {
        _message->arg_types[arg_index] = MQIPC::Message::ArgType::STRING;
        strncpy(
            _message->args[arg_index].value.string_val
            , std::get<std::string>(argv).c_str()
            , sizeof(_message->args[arg_index].value.string_val));
        return;
    }
    else
    {
        throw std::runtime_error("Message argument type not supported!");
    }
}

MQIPC::MessageBuilder::MessageBuilder() : _message(nullptr)
{
    this->reset();
}

MQIPC::MessageBuilder::MessageBuilder(const MQIPC::Message& message) : _message(std::make_unique<MQIPC::Message>(message))
{

}

void MQIPC::MessageBuilder::reset()
{
    _message = std::make_unique<MQIPC::Message>();
}

void MQIPC::MessageBuilder::reset(const MQIPC::Message& message)
{
    _message = std::make_unique<MQIPC::Message>(std::move(message));
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_type(MQIPC::Message::Type msg_type)
{
    _message->type = msg_type;
    return *this;
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_sender(const char* msg_sender)
{
    if (strlen(msg_sender) >= sizeof(_message->from))
    {
        throw std::length_error("sender name too long!");
    }
    strncpy(_message->from, msg_sender, sizeof(_message->from));
    return *this;
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_receiver(const char* msg_receiver)
{
    if (strlen(msg_receiver) >= sizeof(_message->to))
    {
        throw std::length_error("receiver name too long!");
    }
    strncpy(_message->to, msg_receiver, sizeof(_message->to));
    return *this;
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_content(const char* msg_content)
{
    if (strlen(msg_content) >= sizeof(_message->content))
    {
        throw std::length_error("receiver name too long!");
    }
    strncpy(_message->content, msg_content, sizeof(_message->content));
    return *this;
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_correlation_id(const char* id)
{
    if (strlen(id) >= sizeof(_message->content))
    {
        throw std::length_error("correlation_id too long!");
    }
    strncpy(_message->correlation_id, id, sizeof(_message->correlation_id));
    return *this;
}

MQIPC::MessageBuilder& MQIPC::MessageBuilder::add_args(const std::vector<MQIPC::ArgVariant>& arg_vec)
{
    if (arg_vec.size() > MQIPC::Message::MAX_ARGS)
    {
        throw std::runtime_error("Too many arguments for message!");
    }
    if (arg_vec.size() <= 0)
    {
        return *this;
    }
    _message->argc = arg_vec.size();
    for (size_t i = 0; i < arg_vec.size(); i++)
    {
        set_arguments(i, arg_vec[i]);
    }

    return *this;
}

std::unique_ptr<MQIPC::Message> MQIPC::MessageBuilder::message()
{
    auto res = std::move(_message);  // Move ownership
    reset();
    return res;
}