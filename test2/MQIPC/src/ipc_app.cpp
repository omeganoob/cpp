#include <poll.h>
#include "ipc_app.h"

MQIPC::App::App(const std::string& client_name, const std::string& server, int permisson)
    : _server_name(server)
    , _client_name(client_name)
    , _queue_permission(permisson)
    , _queue_max_msg(10)
    , _queue_msg_sz(MQIPC::MSG_SZ)
    , _queue_max_prio(9)
    , _max_retries(3)
{
}

MQIPC::App::~App()
{
    stop();
    disconnect();
}

bool MQIPC::App::connect(ulong wait_time)
{
    try
    {
        _server_mqueue = std::make_unique<MQueueHandler>("/" + _server_name, O_WRONLY);

        mq_attr attr;

        if (!_server_mqueue->get_attributes(&attr)) {
            // Handle error or use defaults
            attr.mq_flags = 0;
            attr.mq_maxmsg = _queue_max_msg;
            attr.mq_msgsize = _queue_msg_sz;
            attr.mq_curmsgs = 0;
        }

        _client_mqueue = std::make_unique<MQueueHandler>(
            "/" + _client_name,
            O_CREAT | O_RDWR | O_EXCL,
            _queue_permission,
            &attr,
            true
        );

        auto msg = MQIPC::MessageBuilder{}
            .add_type(MQIPC::Message::Type::CONNECT)
            .add_sender(_client_name.c_str())
            .add_receiver(_server_name.c_str())
            .message();

        if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQueueHandler::SendResult::Success)
        {
            throw std::runtime_error("Failed to send registration: " + _server_mqueue->get_last_error());
        }

        Message response;

        if (_client_mqueue->timed_receive(reinterpret_cast<char*>(&response), MQIPC::MSG_SZ, 0, wait_time) != MQueueHandler::ReceiveResult::Success)
        {
            throw std::runtime_error("Failed to receive acknowlegdement: " + _client_name + " " + _client_mqueue->get_last_error());
        }

        if (response.type == MQIPC::Message::Type::RESPONSE)
        {
            tcolor::Printer::execute(tcolor::fgreen, "Successfully connect as ", _client_name);
        }
    }
    catch (const std::exception& e)
    {
        tcolor::Printer::execute(tcolor::fred, "Connection failed: ", e.what());
        return false;
    }

    return true;
}

void MQIPC::App::dispatch_message(std::function<void(MQIPC::Message)> handler)
{
    if (_running)
    {
        return;
    }

    auto new_handler = std::make_shared<std::function<void(Message)>>(handler);
    std::atomic_store(&_message_handler, new_handler);
    _running.exchange(true);
    _shutdown_requested = false;

    for (size_t i = 0; i < _num_workers; i++)
    {
        _workers.emplace_back(&App::worker_thread, this, i);
    }
    tcolor::Printer::execute(tcolor::fyellow, "Running with ", _num_workers, " workers...");
}

bool MQIPC::App::call_method(const std::string& service, const std::string& method, const std::vector<MQIPC::ArgVariant>& args, bool async)
{
    auto msg = MQIPC::MessageBuilder{}
        .add_type(async ? MQIPC::Message::Type::ASYNC_CALL : MQIPC::Message::Type::METHOD_CALL)
        .add_sender(_client_name.c_str())
        .add_receiver(service.c_str())
        .add_content(method.c_str())
        .add_args(args)
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error call_method(", service, ".", method, ", ", async ? "true" : "false", ")");
        return false;
    }

    if (!async)
    {
        MQIPC::Message reply;

        if (_client_mqueue->timed_receive(reinterpret_cast<char*>(&reply), MQIPC::MSG_SZ, 0, 1000) != MQIPC::MQueueHandler::ReceiveResult::Success)
        {
            tcolor::Printer::execute(tcolor::fred, "Failed to receive reply: ", _client_name);
            return false;
        }

        if (reply.type == MQIPC::Message::Type::METHOD_RETURN)
        {
            tcolor::Printer::execute(tcolor::fcyan, "Received method return: ", reply.from, ".", reply.content);
            auto current_handler = std::atomic_load(&_message_handler);
            if (current_handler)
            {
                (*_message_handler)(reply);
            }
            else
            {
                tcolor::Printer::execute(tcolor::fyellow, reply.from, ".", reply.content, " returned. But no handler was registered. Try to dispatch_message().");
            }
            return true;
        }
    }
    return true;
}

bool MQIPC::App::method_return(const std::string& caller, const MQIPC::Message& msg, const std::vector<MQIPC::ArgVariant>& args)
{
    auto return_msg = MQIPC::MessageBuilder{ msg }
        .add_type(MQIPC::Message::Type::METHOD_RETURN)
        .add_sender(_client_name.c_str())
        .add_receiver(caller.c_str())
        .add_args(args)
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(return_msg.get()), MQIPC::MSG_SZ, 1) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error send return: ", _server_mqueue->get_last_error());
        return false;
    }
    return true;
}

bool MQIPC::App::register_service(const std::string& method_name, ulong wait_time)
{
    auto msg = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::Type::REGISTRY_SERVICE)
        .add_sender(_client_name.c_str())
        .add_receiver(_server_name.c_str())
        .add_content(method_name.c_str())
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error sending registration: ", _client_name, ".", method_name, " ", _server_mqueue->get_last_error());
        return false;
    }

    MQIPC::Message response;

    if (_client_mqueue->timed_receive(reinterpret_cast<char*>(&response), MQIPC::MSG_SZ, 0, wait_time) != MQIPC::MQueueHandler::ReceiveResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to receive acknowledgment: ", _client_name, ".", method_name, " ", _client_mqueue->get_last_error());
        return false;
    }

    if (response.type == MQIPC::Message::Type::RESPONSE)
    {
        tcolor::Printer::execute(tcolor::fgreen, "Successfully registered method ", _client_name, ".", method_name);
        return true;
    }

    return false;
}

bool MQIPC::App::subscribe_service(const std::string& service_name, const std::string& method_name, ulong wait_time)
{
    auto msg = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::Type::SUBSCRIBE)
        .add_sender(_client_name.c_str())
        .add_receiver(service_name.c_str())
        .add_content(method_name.c_str())
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error request subscription: ", service_name, ".", method_name, " ", _server_mqueue->get_last_error());
        return false;
    }

    MQIPC::Message response;

    if (_client_mqueue->timed_receive(reinterpret_cast<char*>(&response), MQIPC::MSG_SZ, 0, wait_time) != MQIPC::MQueueHandler::ReceiveResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to receive subscription confirm: ", service_name, ".", method_name, " ", _client_mqueue->get_last_error());
        return false;
    }

    if (response.type == MQIPC::Message::Type::RESPONSE)
    {
        tcolor::Printer::execute(tcolor::fgreen, "Successfully subscribe: ", service_name, ".", method_name);
        return true;
    }
    return false;
}

bool MQIPC::App::unsubscribe_service(const std::string& service_name, const std::string& method_name, ulong wait_time)
{
    auto msg = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::Type::UNSUBSCRIBE)
        .add_sender(_client_name.c_str())
        .add_receiver(service_name.c_str())
        .add_content(method_name.c_str())
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error request unsubscription: ", service_name, ".", method_name, " ", _server_mqueue->get_last_error());
        return false;
    }

    MQIPC::Message response;

    if (_client_mqueue->timed_receive(reinterpret_cast<char*>(&response), MQIPC::MSG_SZ, 0, wait_time) != MQIPC::MQueueHandler::ReceiveResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to receive unsubscription confirm: ", service_name, ".", method_name, " ", _client_mqueue->get_last_error());
        return false;
    }

    if (response.type == MQIPC::Message::Type::RESPONSE)
    {
        tcolor::Printer::execute(tcolor::fgreen, "Successfully unsubscribe: ", service_name, ".", method_name);
        return true;
    }
    return false;
}

bool MQIPC::App::send_signal(bool broacasting, const std::string& signal, const std::vector<MQIPC::ArgVariant>& args)
{

    auto send_signal = MQIPC::MessageBuilder{}
        .add_type(broacasting ? MQIPC::Message::Type::BROADCAST_SIGNAL : MQIPC::Message::Type::SUBSCRIBE_SIGNAL)
        .add_sender(_client_name.c_str())
        .add_content(signal.c_str())
        .add_args(args)
        .message();

    if (_server_mqueue->send(reinterpret_cast<char*>(send_signal.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Error sending signal: ", _client_name, ".", signal, " ", _server_mqueue->get_last_error());
        return false;
    }
    return true;
}

bool MQIPC::App::disconnect()
{
    auto msg = MQIPC::MessageBuilder{}
        .add_type(MQIPC::Message::Type::DISCONNECT)
        .add_sender(_client_name.c_str())
        .message();
    if (_server_mqueue->send(reinterpret_cast<char*>(msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to send disconnect request: ", _client_name, " ", _server_mqueue->get_last_error());
        return false;
    }
    return true;
}

void MQIPC::App::set_max_retries(uint value)
{
    if (value >= 10)
    {
        tcolor::Printer::execute(tcolor::fyellow, "Max retries possible is 10");
        return;
    }
    _max_retries = value;
}

void MQIPC::App::set_num_workers(size_t num)
{
    if (_running)
    {
        tcolor::Printer::execute(tcolor::fred, "[set_num_workers] Abort: ", "Cannot change number of worker while app is running.");
        return;
    }

    if (_num_workers > std::thread::hardware_concurrency() - 2)
    {
        tcolor::Printer::execute(tcolor::fred, "[set_num_workers] Abort: ", "Too many worker. Device performace may be decreased.");
        return;
    }
    _num_workers = num;
}

void MQIPC::App::worker_thread(int id)
{
    struct pollfd fds[1];
    fds[0].fd = _client_mqueue->get();
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

        if(!_running) break;

        int ret = poll(fds, 1, 100);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            tcolor::Printer::execute(tcolor::fred, "Poll error: ", strerror(errno));
            break;
        }

        // Check if there's data to read
        if (ret > 0 && (fds[0].revents & POLLIN))
        {
            MQIPC::Message msg;
            if (_client_mqueue->receive(reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::ReceiveResult::Success)
            {
                tcolor::Printer::execute(tcolor::fred, "Error receiving message: ", _client_mqueue->get_last_error());
                continue;
            }

            if (msg.type == MQIPC::Message::Type::TERMINATE)
            {
                // Set shutdown flag
                {
                    std::lock_guard<std::mutex> lock(_shutdown_mutex);
                    _shutdown_requested = true;
                }
                // Forward the terminate message to wake up other workers
                _client_mqueue->send(reinterpret_cast<char*>(&msg), MQIPC::MSG_SZ, 0);
                break;
            }
            
            auto current_handler = std::atomic_load(&_message_handler);
            if (current_handler && _running)
            {
                (*current_handler)(msg);
            }
        }
    }

    tcolor::Printer::execute(tcolor::fmagenta, "Worker ", id, " Stopping...");
}

void MQIPC::App::stop()
{
    if (!_running.exchange(false))
    {
        return;
    }

    // Signal shutdown
    {
        std::lock_guard<std::mutex> lock(_shutdown_mutex);
        _shutdown_requested = true;
    }

    // Send terminate message to wake up blocked workers
    auto wakeup_msg = MQIPC::MessageBuilder{}
        .add_sender("self")
        .add_receiver("self")
        .add_type(MQIPC::Message::Type::TERMINATE)
        .add_content("stop signal")
        .message();

    if (_client_mqueue->send(reinterpret_cast<char*>(wakeup_msg.get()), MQIPC::MSG_SZ, 0) != MQIPC::MQueueHandler::SendResult::Success)
    {
        tcolor::Printer::execute(tcolor::fred, "Failed to send stop signal: ", _client_mqueue->get_last_error());
    }
    // Wait for all workers to finish
    for (std::thread& worker : _workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    _workers.clear();
    _message_handler = nullptr;
}
