#include <poll.h>
#include <algorithm>
#include "ipc_server.h"
#include "t_color.h"

MQIPC::Server::Server(
    std::string server_name,
    int permission,
    long max_msg,
    int prio,
    size_t num_workers)
    : _server_name(server_name)
    , _queue_permission(permission)
    , _queue_max_msg(max_msg)
    , _queue_msg_sz(MQIPC::MSG_SZ)
    , _queue_max_prio(prio)
    , _num_workers(num_workers)
    , _max_retries(3)
{
    tcolor::Printer::execute(tcolor::fgreen, "Server created with name: ", _server_name);
}

MQIPC::Server::~Server()
{
    stop();
}

void MQIPC::Server::start()
{
    try
    {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = _queue_max_msg;
        attr.mq_msgsize = _queue_msg_sz;
        attr.mq_curmsgs = 0;

        _server_mqueue = std::make_unique<MQueueHandler>("/" + _server_name, O_CREAT | O_RDWR, _queue_permission, &attr, true);

    }
    catch (const std::exception& e)
    {
        tcolor::Printer::execute(tcolor::fred, "Error starting server: ", e.what());
    }

    tcolor::Printer::execute(tcolor::fgreen, "Server started with queue: /", _server_name);
    run();
}

void MQIPC::Server::run()
{
    _running.exchange(true);

    for (size_t i = 0; i < _num_workers; i++)
    {
        _workers.emplace_back(&Server::worker_thread, this, i);
    }

    tcolor::Printer::execute(tcolor::fyellow, "Server running with ", _num_workers, " workers...");

    std::thread terminal(&Server::terminal_thread, this);
    terminal.detach();

    for (std::thread& worker : _workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void MQIPC::Server::stop()
{

    if (_stopping.exchange(true)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_shutdown_mutex);
        _shutdown_requested = true;
    }

    send_shutdown_notification();

    _running = false;

    // Send a "wake-up" message to unblock any worker waiting in poll
    auto wakeup_msg = MQIPC::MessageBuilder{}
        .add_sender("self")
        .add_receiver("self")
        .add_type(MQIPC::Message::TERMINATE)
        .add_content("stop signal")
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(wakeup_msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to send stop signal: ", _server_mqueue->get_last_error());
    }

    dispatch_pending_msg();
    send_shutdown_notification();

    for (std::thread& worker : _workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    _workers.clear();
}

void MQIPC::Server::handle_connection(const MQIPC::Message& msg)
{
    std::string client_name(msg.from);

    uint retries = _max_retries;

    std::unique_ptr<MQueueHandler> client_queue{ nullptr };

    if (_client_queues.find(client_name) != _client_queues.end())
    {
        tcolor::Printer::execute(tcolor::fred, "Connection refused: queue ", client_name, " existed!");
        return;
    }

    // Open .client's queue for response - write only
    while (retries-- > 0)
    {
        try
        {
            client_queue = std::make_unique<MQueueHandler>("/" + client_name, O_WRONLY);
        }
        catch (const std::exception& e)
        {
            tcolor::Printer::execute(tcolor::fred, "Failed to open: ", client_name, e.what());
        }

        if (client_queue->is_valid())
        {
            break;
        }
        usleep(50'000);
    }
    _client_queues[client_name] = std::move(client_queue);

    // Reponse that we have successfully establish connection
    auto response = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::Type::RESPONSE)
        .add_sender(_server_name.c_str())
        .message();


    if (_client_queues[client_name]->send( reinterpret_cast<char*>(response.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed send connection response to client: ", client_name, " ", strerror(errno));
        return;
    }
}

void MQIPC::Server::handle_service_registration(const MQIPC::Message& msg)
{
    std::string service(msg.from);
    std::string method(msg.content);

    _register_services[service].push_back(service + "." + method);

    auto response = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::RESPONSE)
        .add_sender(_server_name.c_str())
        .add_receiver(service.c_str())
        .message();

    if (_client_queues[service]->send(reinterpret_cast<char*>(response.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Failed send register response to client: ", service, ".", method, " ", _client_queues[service]->get_last_error());
        return;
    }
}

void MQIPC::Server::handle_method_call(const MQIPC::Message& msg)
{
    std::string receiver(msg.to);
    std::string caller(msg.from);
    std::string method(msg.content);

    tcolor::Printer::execute(tcolor::fcyan, caller, " Call method: [", method, "] from: ", receiver);

    bool found = false;

    for (const auto& [client, services] : _register_services)
    {
        auto it = std::find(services.begin(), services.end(), receiver + "." + method);
        if (it != services.end())
        {
            std::unique_ptr<MQIPC::Message> forward_msg;
            std::string call_id = generate_call_id();

            auto msg_builder = MQIPC::MessageBuilder{ msg };
            if (msg.type == MQIPC::Message::Type::METHOD_CALL)
            {
                _pending_calls[call_id] = PendingMethodCall{
                    .caller = caller,
                    .method = method,
                    .timestamp = std::chrono::steady_clock::now() };
                msg_builder.add_correlation_id(call_id.c_str());
            }

            forward_msg = msg_builder.message();

            if (_client_queues[client]->send(reinterpret_cast<char*>(forward_msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
            {
                tcolor::Printer::execute(tcolor::fred, "Error forwarding method call: ", _client_queues[client] -> get_last_error());
                _pending_calls.erase(call_id);
            }
            else
            {
                tcolor::Printer::execute(tcolor::fgreen, "Method forwarded: ", client, ", ", *it);
                found = true;
            }
        }
    }

    if (!found)
    {
        tcolor::Printer::execute(tcolor::fred, "No registered service: ", receiver, ".", method);
    }

    cleanup_pending_call();
}

void MQIPC::Server::handle_broadcast_signal(MQIPC::Message& msg)
{
    std::string emitter(msg.from);
    std::string signal(msg.content);

    tcolor::Printer::execute(tcolor::fcyan, emitter, " Emitting broadcast signal: [", signal, "]");

    for (auto&& [client_name, client_queue] : _client_queues)
    {
        if (client_name == emitter) continue;
        uint8_t retries = 3;
        while (retries-- > 0)
        {
            if (client_queue->send(reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
            {
                tcolor::Printer::execute(tcolor::fred, "Error forwarding signal from ", emitter, " to ", client_name, ": ", client_queue->get_last_error());
                continue;
            }
            else
            {
                tcolor::Printer::execute(tcolor::fgreen, "Forwarded signal from ", emitter, " to ", client_name);
                break;
            }
            usleep(50'000);
        }
    }
}

void MQIPC::Server::handle_subscription_signal(MQIPC::Message& msg)
{
    std::string emitter(msg.from);
    std::string signal(msg.content);
    std::string r_service = std::string(emitter + "." + signal);

    std::vector<std::string> subscribers;

    try
    {
        subscribers = _pub_subs[r_service];
        if (subscribers.size() <= 0)
        {
            throw std::runtime_error("No subscribers available.");
        }

        tcolor::Printer::execute(tcolor::fcyan, emitter, " Emitting subsccription signal: [", signal, "]");

        for (auto&& subscriber : subscribers)
        {
            uint8_t retries = 3;
            while (retries-- > 0)
            {
                if (_client_queues[subscriber]->send( reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
                {
                    tcolor::Printer::execute(tcolor::fred, "Error forwarding signal from ", emitter, " to ", subscriber, ": ", _client_queues[subscriber]->get_last_error());
                    continue;
                }
                else
                {
                    tcolor::Printer::execute(tcolor::fgreen, "Forwarded signal from ", emitter, " to ", subscriber);
                    break;
                }
                usleep(50'000);
            }
        }
    }
    catch (const std::exception& e)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Error : ", r_service, " has no subscribers(skipping): ", e.what());
        return;
    }
}

void MQIPC::Server::handle_subscription(MQIPC::Message& msg)
{
    std::string subscriber(msg.from);
    std::string service(msg.to);
    std::string method(msg.content);
    std::string request_service = std::string(service + "." + method);
    try
    {
        std::vector<std::string>& available_method = _register_services.at(service);
        if (std::find(available_method.cbegin(), available_method.cend(), method) != available_method.end())
        {
            _pub_subs[request_service].push_back(subscriber);
        }
        else
        {
            throw std::runtime_error("Service unavailable");
        }
    }
    catch (const std::exception& e)
    {
        tcolor::Printer::execute(tcolor::fred, "Error subscribing: ", subscriber, " to ", request_service, ": ", e.what());
        return;
    }

    auto response = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::RESPONSE)
        .add_sender(_server_name.c_str())
        .add_receiver(subscriber.c_str())
        .message();

    if (_client_queues[subscriber]->send( reinterpret_cast<char*>(response.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Failed confirm subscription from client: ", subscriber, ": ", request_service, " ", _client_queues[subscriber]->get_last_error());
        return;
    }
}

void MQIPC::Server::handle_unsubscription(MQIPC::Message& msg)
{
    std::string subscriber(msg.from);
    std::string service(msg.to);
    std::string method(msg.content);
    std::string request_service = std::string(service + "." + method);

    try
    {
        std::vector<std::string>& subscriber_list = _pub_subs.at(request_service);
        std::vector<std::string>::const_iterator _subscriber = std::find(subscriber_list.cbegin(), subscriber_list.cend(), subscriber);
        if (_subscriber != subscriber_list.end())
        {
            subscriber_list.erase(_subscriber);
            if (subscriber_list.size() <= 0)
            {
                _pub_subs.erase(request_service);
            }
        }
        else
        {
            throw std::out_of_range("Never subscribed");
        }
    }
    catch (const std::exception& e)
    {
        tcolor::Printer::execute(tcolor::fred, "Error unsubscribing: ", subscriber, " from ", request_service, ": ", e.what());
    }

    auto response = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::RESPONSE)
        .add_sender(_server_name.c_str())
        .add_receiver(subscriber.c_str())
        .message();

    if (_client_queues[subscriber]->send(reinterpret_cast<char*>(response.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Failed unsubscribe client: ", subscriber, ": ", request_service, " ", _client_queues[subscriber]->get_last_error());
        return;
    }
}

void MQIPC::Server::handle_disconnect(const MQIPC::Message& msg)
{
    std::string client_name(msg.from);

    if (_client_queues.count(client_name))
    {
        _client_queues.erase(client_name);
    }
    
    _register_services.erase(client_name);
    
    std::cout << tcolor::fyellow << "App disconnected: " << client_name << tcolor::reset << "\n";
}

void MQIPC::Server::handle_method_return(const MQIPC::Message& msg)
{
    std::string call_id(msg.correlation_id);

    tcolor::Printer::execute(tcolor::fcyan, msg.from, " Returned method: [", msg.content, "] to: ", msg.to);

    auto iter = _pending_calls.find(call_id);
    if (iter == _pending_calls.end())
    {
        tcolor::Printer::execute(tcolor::fred, "Received return for unknown call ID: ", call_id);
        return;
    }

    std::unique_ptr<MQIPC::Message> return_msg = MQIPC::MessageBuilder{ msg }.add_receiver(iter->second.caller.c_str()).message();

    if (_client_queues[iter->second.caller]->send( reinterpret_cast<char*>(return_msg.get()), MQIPC::MSG_SZ, 1) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error forwarding method return: ", call_id, ": ", _client_queues[iter->second.caller]->get_last_error());
    }

    _pending_calls.erase(iter);
}

void MQIPC::Server::dispatch_pending_msg()
{
    // Wait for pending calls to complete or timeout
    auto shutdown_start = std::chrono::steady_clock::now();
    while (!_pending_calls.empty() &&
        std::chrono::steady_clock::now() - shutdown_start <
        std::chrono::seconds(10)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MQIPC::Server::send_shutdown_notification()
{
    std::lock_guard<std::mutex> client_lock(_clients_mutex);
    for (const auto& [client_name, client_queue] : _client_queues) {
        try
        {
            auto shutdown_msg = MQIPC::MessageBuilder{}
                .add_type(MQIPC::Message::Type::DISCONNECT)
                .add_sender(_server_name.c_str())
                .add_receiver(client_name.c_str())
                .add_content("SERVER_SHUTDOWN")
                .message();
            std::cout << "\t" << "sending: " << client_name << "\n";
            if (client_queue->send( reinterpret_cast<char*>(shutdown_msg.get()), MQIPC::MSG_SZ, 1) != MQIPC::MQueueHandler::SendResult::Success)
            {
                int send_errno = client_queue->get_errno();
                if ( send_errno == EAGAIN || send_errno == EINTR) {
                    // Queue full or interrupted - wait and retry
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
                if (send_errno == EBADF || send_errno == EINVAL) {
                    throw std::runtime_error("App queue no longer valid" + client_queue->get_last_error());
                }
                throw std::runtime_error("Failed to send shutdown notification: " + client_queue->get_last_error());
            }
        }
        catch (const std::exception& e)
        {
            tcolor::Printer::execute("Error notify shutdown: ", e.what());
        }
    }
}

void MQIPC::Server::worker_thread(uint id)
{
    // Set up polling
    struct pollfd fds[1];
    fds[0].fd = _server_mqueue->get();
    fds[0].events = POLLIN;

    tcolor::Printer::execute(tcolor::fmagenta, "Worker ", id, " Running...");

    while (_running)
    {
        {
            std::unique_lock<std::mutex> lock(_shutdown_mutex);
            if (_shutdown_requested) {
                break;
            }
        }

        if (!_running) break;

        int ret = poll(fds, 1, 100);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;  // Interrupted by signal, try again
            }
            tcolor::Printer::execute(tcolor::fred, "Poll error: ", strerror(errno));
            break;
        }

        if (ret > 0 && (fds[0].revents & POLLIN))
        {
            MQIPC::Message msg;
            if (_server_mqueue->receive(reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::ReceiveResult::Success)
            {
                if (_server_mqueue->get_errno() == EAGAIN) {
                    continue;  // No message available; go back to waiting
                }
                tcolor::Printer::execute(tcolor::fred, "Error receiving message: ", _server_mqueue->get_last_error());
                continue;
            }

            tcolor::Printer::execute(tcolor::fyellow
                , "Workers ", id, " processing message: ", msg.from
                , " --", msg.content
                , "--> ", msg.to
                , ". Type: ", msg.type);

            switch (msg.type)
            {
            case MQIPC::Message::Type::CONNECT:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_connection(msg);
                break;
            }
            case MQIPC::Message::Type::DISCONNECT:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_disconnect(msg);
                break;
            }
            case MQIPC::Message::Type::REGISTRY_SERVICE:
            {
                std::lock_guard<std::mutex> lock(_services_mutex);
                handle_service_registration(msg);
                break;
            }
            case MQIPC::Message::Type::SUBSCRIBE:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_subscription(msg);
                break;
            }
            case MQIPC::Message::Type::UNSUBSCRIBE:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_subscription(msg);
                break;
            }
            case MQIPC::Message::Type::BROADCAST_SIGNAL:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_broadcast_signal(msg);
                break;
            }
            case MQIPC::Message::Type::SUBSCRIBE_SIGNAL:
            {
                std::lock_guard<std::mutex> lock(_clients_mutex);
                handle_subscription_signal(msg);
                break;
            }
            case MQIPC::Message::Type::METHOD_CALL:
            case MQIPC::Message::Type::ASYNC_CALL:
            {
                std::lock_guard<std::mutex> service_lock(_services_mutex);
                std::lock_guard<std::mutex> pending_lock(_pending_calls_mutex);
                handle_method_call(msg);
                break;
            }
            case MQIPC::Message::Type::METHOD_RETURN:
            {
                std::lock_guard<std::mutex> pending_lock(_pending_calls_mutex);
                handle_method_return(msg);
                break;
            }
            case MQIPC::Message::Type::TERMINATE:
            {
                {
                    std::lock_guard<std::mutex> lock(_shutdown_mutex);
                    _shutdown_requested = true;
                }
                if(_server_mqueue->send(reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
                {
                    tcolor::Printer::execute(tcolor::fred, "Error forwarding terminate signal between threads: ", _server_mqueue->get_last_error());
                }
                break;
            }
            default:
                break;
            }
        }
    }
    tcolor::Printer::execute(tcolor::fmagenta, "Worker ", id, " Stopping...");
}

void MQIPC::Server::terminal_thread()
{
    std::string input;
    while (_running) {
        tcolor::Printer::execute(tcolor::fblue, "Input: ");
        std::getline(std::cin, input);
        if (input == "stop") {
            _running = false;
            std::cout << "Stopping server..." << std::endl;
            stop();
            continue;
        }
        if (input == "SERVICES")
        {
            for (auto&& [service, methods] : _register_services)
            {
                std::cout << service << ": \n";
                for (auto&& method : methods)
                {
                    std::cout << "\t" << method << "\n";
                }
            }
            continue;
        }
    }
}

void MQIPC::Server::cleanup_pending_call()
{
    auto now = std::chrono::steady_clock::now();
    auto iter = _pending_calls.begin();
    while (iter != _pending_calls.end())
    {
        if (now - iter->second.timestamp > _method_call_timeout)
        {
            tcolor::Printer::execute(tcolor::fyellow, "Method call time out: ", iter->second.method);
            iter = _pending_calls.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

std::string MQIPC::Server::generate_call_id()
{
    static std::atomic<uint64_t> counter{ 0 };
    return std::to_string(++counter);
}

void MQIPC::Server::set_max_retries(uint value)
{
    if (value >= 10)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Max retries possible is 10");
        _max_retries = 10;
        return;
    }
    _max_retries = value;
}

void MQIPC::Server::set_num_workers(size_t num)
{
    if (_running)
    {
        tcolor::Printer::execute(tcolor::fred, "[set_num_workers] Abort: ", "Cannot change number of worker while server is running.");
        return;
    }

    if (_num_workers > std::thread::hardware_concurrency() - 2)
    {
        tcolor::Printer::execute(tcolor::fred, "[set_num_workers] Abort: ", "Too many worker. Device performace may be decreased.");
        return;
    }
    _num_workers = num;
}
