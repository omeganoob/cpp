#pragma once

#include <list>
#include <map>
#include <mqueue.h>
// #include <atomic>
#include "communication.h"

namespace KIPC
{
    class Server
    {
    public:
        Server(std::string name = "sever_test", int access = 0644);
        ~Server();

        void start();
        void run();
        void stop();

    private:
        void handle_message(const Message &msg);
        void send_respond(const std::string &receiver, const std::string &message);

        mqd_t m_server_queue;
        std::map<std::string, mqd_t> m_client_queues;
        std::map<std::string, std::list<std::string>> m_service_registers;
        // std::map<std::string, std::string> m_client_to_service;
        std::map<std::string, std::list<std::string>> m_service_subscribers;

        // std::atomic<bool> m_running{false};

        const int m_queue_access;
        const int m_queue_max_capacity{100};
        const int m_queue_max_msg_size{256};
    };
}