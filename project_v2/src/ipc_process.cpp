#include <iostream>
#include <cstring>
#include <poll.h>
#include "ipc_process.h"

namespace KIPC
{
    Client::Client(const std::string &client_name, const std::string &server_name, int access) : m_client_name(client_name), m_server(server_name), m_queue_access(access)
    {
        m_client_name = "/" + client_name;
        m_server = "/" + server_name;
    }

    Client::~Client()
    {
        disconnect();
    }

    bool Client::connect()
    {
        m_server_queue = mq_open(m_server.c_str(), O_WRONLY);
        if (m_server_queue == -1)
        {
            std::cerr << "Failed to open server queue: " << strerror(errno) << "\n";
            return false;
        }

        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = m_queue_max_capacity;
        attr.mq_msgsize = m_queue_max_msg_size;
        attr.mq_curmsgs = 0;

        m_client_queue = mq_open(m_client_name.c_str(), O_CREAT | O_RDONLY, m_queue_access, &attr);
        if (m_client_queue == -1)
        {
            std::cerr << "Failed to create client queue: " << strerror(errno) << "\n";
            mq_close(m_server_queue);
            return false;
        }

        // if (mq_getattr(m_client_queue, &attr) == 0)
        // {
        //     std::cout << "Max message size (mq_msgsize): " << attr.mq_msgsize << std::endl;
        //     std::cout << "Current struct Message size: " << sizeof(Message) << std::endl;
        // }
        // else
        // {
        //     std::cerr << "Failed to get message queue attributes: " << strerror(errno) << std::endl;
        // }

        if (!send_message(m_server, Message::MessageType::CONNECT, "", ""))
        {
            std::cerr << "Failed to send connect message to server: " << strerror(errno) << "\n";
            mq_close(m_client_queue);
            mq_close(m_server_queue);
            mq_unlink(m_client_name.c_str());
            return false;
        }

        Message response;
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 5;

        if (mq_timedreceive(m_client_queue, (char *)&response, m_queue_max_msg_size, nullptr, &timeout) == -1)
        {
            if (errno == ETIMEDOUT)
            {
                std::cerr << "Timeout: Server did not respond. Possible duplicate name.\n";
            }
            else
            {
                std::cerr << "Failed to receive server response: " << strerror(errno) << "\n";
            }
            return false;
        }

        // Xử lý phản hồi từ server
        if (strncmp(response.parameters, "Connect successfully", sizeof(response.parameters)) == 0)
        {
            std::cout << "Connected to server successfully.\n";
        }
        else
        {
            std::cerr << "Server returned an unexpected response: " << response.parameters << "\n";
            return false;
        }

        // std::cout << "Client : " << m_client_name << " connected to server: " << m_server << "\n";
        return true;
    }

    bool Client::call_method(const std::string &service_name, const std::string &method_name, const std::string &arguments)
    {
        std::cout << "Sending method call for service: " << service_name << ", method: " << method_name << "\n";

        if (!send_message(m_server, Message::MessageType::METHOD_CALL, method_name, arguments))
        {
            std::cerr << "Failed send method call: " << strerror(errno) << "\n";
            return false;
        }

        return true;
    }

    bool Client::register_service(const std::string &service_name, const std::string &method_name)
    {
        Message message;
        Message respond;

        message.type = Message::MessageType::REGISTER_SERVICE;
        strncpy(message.sender, m_client_name.c_str(), sizeof(message.sender) - 1);
        strncpy(message.method, method_name.c_str(), sizeof(message.method) - 1);

        if (!send_message_and_wait_response(message, respond, 5))
        {
            std::cerr << "Failed to send register service message: " << strerror(errno) << "\n";
            return false;
        }

        if (strcmp(respond.parameters, "Method already registered") == 0)
        {
            std::cout << "Error: Method already ready registered\n";
            return true;
        }
        else if (strcmp(respond.parameters, "Service registered successfully") == 0)
        {
            std::cerr << "Register success service method: " << method_name << "\n";
            return true;
        }
        else
        {
            std::cerr << "Failed to register method: " << respond.parameters << "\n";
            return false;
        }
    }

    bool Client::subscribe_service(const std::string &service_name)
    {
        Message message;
        message.type = Message::MessageType::SUBSCRIBE;
        strncpy(message.sender, m_client_name.c_str(), sizeof(message.sender) - 1);
        strncpy(message.receiver, service_name.c_str(), sizeof(message.receiver) - 1);

        Message response;
        if (!send_message_and_wait_response(message, response, 5)) // Timeout 5 giây
        {
            return false;
        }

        if (strcmp(response.parameters, "Successfully subscribed") == 0)
        {
            std::cout << "Subscribed to service successfully\n";
            return true;
        }
        else if (strcmp(response.parameters, "Already subscribed") == 0)
        {
            std::cerr << "Already subscribed to service\n";
            return false;
        }
        else
        {
            std::cerr << "Failed to subscribe to service: " << response.parameters << "\n";
            return false;
        }
    }

    bool Client::unsubscribe_service(const std::string &service_name)
    {
        Message message;
        message.type = Message::MessageType::UNSUBSCRIBE;
        strncpy(message.sender, m_client_name.c_str(), sizeof(message.sender) - 1);
        strncpy(message.receiver, service_name.c_str(), sizeof(message.receiver) - 1);

        Message response;
        if (!send_message_and_wait_response(message, response, 5)) // Timeout 5 giây
        {
            return false;
        }

        if (strcmp(response.parameters, "Successfully unsubscribed") == 0)
        {
            std::cout << "Unsubscribed from service successfully\n";
            return true;
        }
        else if (strcmp(response.parameters, "Not subscribed") == 0)
        {
            std::cerr << "Not subscribed to service\n";
            return false;
        }
        else
        {
            std::cerr << "Failed to unsubscribe from service: " << response.parameters << "\n";
            return false;
        }
    }

    bool Client::send_signal(const std::string &service_name, const std::string &message)
    {
        if (m_server_queue == -1)
        {
            std::cerr << "Server queue not open.\n";
            return false;
        }

        Message signal_msg;
        signal_msg.type = Message::MessageType::SIGNAL;
        strncpy(signal_msg.sender, m_client_name.c_str(), sizeof(signal_msg.sender) - 1);
        strncpy(signal_msg.parameters, message.c_str(), sizeof(signal_msg.parameters) - 1);

        if (mq_send(m_server_queue, reinterpret_cast<const char *>(&signal_msg), sizeof(signal_msg), 0) == -1)
        {
            std::cerr << "Failed to send SIGNAL: " << strerror(errno) << "\n";
            return false;
        }

        std::cout << "Signal sent to service " << service_name << ": " << message << "\n";
        return true;
    }

    bool Client::disconnect()
    {
        send_message(m_server, Message::MessageType::DISCONNECT, "", "");

        if (m_client_queue != -1)
        {
            mq_close(m_client_queue);
            mq_unlink(m_client_name.c_str());
            m_client_queue = -1;
        }

        if (m_server_queue != -1)
        {
            mq_close(m_server_queue);
            m_server_queue = -1;
        }

        std::cout << "Client " << m_client_name << " disconnected from sever: " << m_server << "\n";
        return true;
    }

    bool Client::loop()
    {
        struct pollfd pfd;
        pfd.fd = m_client_queue; // Message queue descriptor
        pfd.events = POLLIN;     // Chờ dữ liệu đến (POLLIN)

        while (true)
        {
            // Chờ sự kiện trên queue (timeout = -1 nghĩa là chờ vô hạn)
            int poll_result = poll(&pfd, 1, -1);
            if (poll_result == -1)
            {
                std::cerr << "Error in poll: " << strerror(errno) << "\n";
                return false;
            }

            // Kiểm tra nếu có dữ liệu đến
            if (pfd.revents & POLLIN)
            {
                Message message;

                ssize_t received_size = mq_receive(m_client_queue, (char *)(&message), m_queue_max_msg_size, nullptr);
                if (received_size == -1)
                {
                    if (errno == EAGAIN) // Không có dữ liệu, tiếp tục chờ
                        continue;

                    std::cerr << "Error receiving message: " << strerror(errno) << "\n";
                    return false;
                }

                switch (message.type)
                {
                case Message::MessageType::METHOD_CALL:
                    std::cout << "Received method call from " << message.sender << "\n";
                    std::cout << "Method: " << message.parameters << "\n";
                    // add later
                    break;

                case Message::MessageType::METHOD_RETURN:
                    std::cout << "Received method return from " << message.sender << "\n";
                    break;

                case Message::MessageType::RESPOND:
                    std::cout << "Received respond: " << message.parameters << " from " << message.sender << "\n";
                    break;

                default:
                    std::cerr << "Unknown message type received\n";
                    return false;
                }
            }
            else if (pfd.revents & POLLERR)
            {
                std::cerr << "Poll error on queue descriptor\n";
                return false;
            }
            else if (pfd.revents & POLLHUP)
            {
                std::cerr << "Queue descriptor was closed\n";
                return false;
            }
        }

        return true;
    }

    bool Client::clear()
    {
        Message message;
        while (mq_receive(m_client_queue, (char *)(&message), sizeof(message), nullptr) != -1)
        {
            // some thing
        }
        if (errno != EAGAIN)
        {
            std::cerr << "Error cleaning messages: " << strerror(errno) << "\n";
            return false;
        }
        return true;
    }

    bool Client::send_message(const std::string &receiver, const Message::MessageType type, const std::string &method, const std::string &parameter)
    {
        Message message;
        message.type = type;
        strncpy(message.sender, m_client_name.c_str(), sizeof(message.sender) - 1);
        strncpy(message.receiver, receiver.c_str(), sizeof(message.receiver) - 1);
        strncpy(message.method, method.c_str(), sizeof(message.method) - 1);
        strncpy(message.parameters, parameter.c_str(), sizeof(message.parameters) - 1);

        // std::cout << "Size of Message struct: " << sizeof(Message) << "\n";
        // std::cout << "______________________________\n";

        if (mq_send(m_server_queue, (char *)(&message), sizeof(message), 0) == -1)
        {
            std::cerr << "Failed to send message: " << strerror(errno) << "\n";
            return false;
        }
        return true;
    }

    bool Client::send_message_and_wait_response(const Message &message, Message &response, int timeout_seconds)
    {
        if (mq_send(m_server_queue, (char *)(&message), sizeof(message), 0) == -1)
        {
            std::cerr << "Failed to send message: " << strerror(errno) << "\n";
            return false;
        }

        struct mq_attr attr;
        mq_getattr(m_client_queue, &attr);

        char buffer[attr.mq_msgsize];
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_seconds;

        ssize_t bytes_read = mq_timedreceive(m_client_queue, buffer, attr.mq_msgsize, nullptr, &ts);
        if (bytes_read == -1)
        {
            if (errno == ETIMEDOUT)
            {
                std::cerr << "Timed out waiting for response\n";
            }
            else
            {
                std::cerr << "Failed to receive message: " << strerror(errno) << "\n";
            }
            return false;
        }

        memcpy(&response, buffer, sizeof(Message));
        return true;
    }
}
