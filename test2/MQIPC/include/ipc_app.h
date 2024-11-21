#pragma once
#include <mqueue.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <functional>
#include <variant>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "queue_handler.h"
#include "t_color.h"
#include "message.h"

namespace MQIPC
{
    class App
    {
    public:
        App(const std::string& client_name, const std::string& server, int permission = 0664);
        ~App();

        bool connect(ulong wait_time = 350);
        void dispatch_message(std::function<void(MQIPC::Message)> handler);
        bool call_method(const std::string& service, const std::string& method, const std::vector<MQIPC::ArgVariant>& args = {}, bool async = false);
        bool method_return(const std::string& caller, const MQIPC::Message& msg, const std::vector<MQIPC::ArgVariant>& args = {});
        bool register_service(const std::string& method_name, ulong wait_time = 350);
        bool subscribe_service(const std::string& service_name, const std::string& method_name, ulong wait_time = 350);
        bool unsubscribe_service(const std::string& service_name, const std::string& method_name, ulong wait_time = 350);
        bool send_signal(bool broacasting, const std::string& signal, const std::vector<MQIPC::ArgVariant>& args = {});
        bool disconnect();
        void set_max_retries(uint value);
        void set_num_workers(size_t num);
        void stop();
    private:

        void worker_thread(int id);

        std::string _client_name;
        std::string _server_name;
        
        std::unique_ptr<MQueueHandler> _server_mqueue;
        std::unique_ptr<MQueueHandler> _client_mqueue;
        
        uint _max_retries;

        const int _queue_permission;
        const long _queue_max_msg;
        const long _queue_msg_sz;
        const int _queue_max_prio;

        std::vector<std::string> services;

        std::vector<std::thread> _workers;
        size_t _num_workers{ 2 };
        std::atomic_bool _running{ false };

        std::shared_ptr<std::function<void(Message)>> _message_handler;
        std::mutex _shutdown_mutex;
        bool _shutdown_requested{ false };
    };
}

