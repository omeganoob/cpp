#pragma once

#include <string>
#include <mqueue.h>
#include "communication.h"

namespace KIPC
{
    class Client
    {
    public:
        Client(const std::string &client_name, const std::string &server, int access = 0664);
        ~Client();

        bool connect();
        bool call_method(const std::string &service_name, const std::string &method_name, const std::string &arguments);
        bool register_service(const std::string &service_name, const std::string &method_name);
        bool subscribe_service(const std::string &service_name);
        bool unsubscribe_service(const std::string &service_name);
        bool send_signal(const std::string &service_name, const std::string &message);
        bool disconnect();
        bool loop();
        bool clear();

    private:
        bool send_message(const std::string &receiver, const Message::MessageType type, const std::string &method, const std::string &parameter);
        bool send_message_and_wait_response(const Message &message, Message &response, int timeout_seconds);

        std::string m_client_name;
        std::string m_server;
        mqd_t m_server_queue;
        mqd_t m_client_queue;

        const int m_queue_access;
        const int m_queue_max_capacity{100};
        const int m_queue_max_msg_size{256};
    };
}