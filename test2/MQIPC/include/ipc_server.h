#pragma once
#include <mqueue.h>
#include <string>
#include <map>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include "queue_handler.h"
#include "message.h"

namespace MQIPC
{
    struct PendingMethodCall
    {
        std::string caller;
        std::string method;
        std::chrono::steady_clock::time_point timestamp;
    };

    class Server
    {
    public:
        Server(std::string server_name = "my_server", int permission = 0644, long max_msg = 100, int prio = 9, size_t num_workers = 4);
        ~Server();
        void start();
        void run();
        void stop();
        void set_max_retries(uint value);
        void set_num_workers(size_t num);
    private:
        //Methods
        void handle_connection(const MQIPC::Message& msg);
        void handle_service_registration(const MQIPC::Message& msg);
        void handle_method_call(const MQIPC::Message& msg);
        void handle_broadcast_signal(MQIPC::Message& msg);
        void handle_subscription_signal(MQIPC::Message& msg);
        void handle_subscription(MQIPC::Message& msg);
        void handle_unsubscription(MQIPC::Message& msg);
        void handle_disconnect(const MQIPC::Message& msg);
        void handle_method_return(const MQIPC::Message& msg);
        void send_shutdown_notification();
        void dispatch_pending_msg();
        void worker_thread(uint id);
        void terminal_thread();
        void cleanup_pending_call();
        std::string generate_call_id();
        //Members
        std::string _server_name;
        uint _max_retries;

        const int _queue_permission;
        const long _queue_max_msg;
        const long _queue_msg_sz;
        const int _queue_max_prio;

        std::unique_ptr<MQueueHandler> _server_mqueue;
        std::map<std::string, std::unique_ptr<MQueueHandler>> _client_queues;

        std::map<std::string, std::vector<std::string>> _register_services;
        std::map<std::string, std::vector<std::string>> _pub_subs;

        std::map<std::string, PendingMethodCall> _pending_calls;
        const std::chrono::milliseconds _method_call_timeout{ 5000 };

        //Concurrency
        std::vector<std::thread> _workers;
        size_t _num_workers{ 4 };
        std::atomic_bool _running{ false };
        std::atomic_bool _stopping{ false };

        std::mutex _clients_mutex;
        std::mutex _services_mutex;
        std::mutex _pending_calls_mutex;
        std::mutex _shutdown_mutex;
        bool _shutdown_requested{false};
    };
}