#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <map>
#include <algorithm>
#include "server_queue.h"

namespace KIPC
{
    Server::Server(std::string name, int access) : m_queue_access(access), m_server_queue(-1)
    {
        std::string sever_queue_name = "/" + name;

        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = m_queue_max_capacity;
        attr.mq_msgsize = m_queue_max_msg_size;
        attr.mq_curmsgs = 0;

        m_server_queue = mq_open(sever_queue_name.c_str(), O_CREAT | O_RDONLY, m_queue_access, &attr);
        if (m_server_queue == -1)
        {
            std::cerr << "Failed to creat server queue: " << strerror(errno) << "\n";
            throw std::runtime_error("Couldn't create server queue");
        }
    }

    Server::~Server()
    {
        stop();
    }

    void Server::start()
    {
        std::cout << "Server started. Waiting for message ... \n";
        run();
    }

    void Server::run()
    {
        char buffer[m_queue_max_msg_size];
        while (true)
        {
            ssize_t bytes_read = mq_receive(m_server_queue, buffer, m_queue_max_msg_size, nullptr);
            if (bytes_read >= 0)
            {
                Message *msg = reinterpret_cast<Message *>(buffer);
                handle_message(*msg);
            }
            else if (errno == EAGAIN)
            {
                continue;
            }
            else
            {
                std::cerr << "Failed to receive message" << strerror(errno) << "\n";
                break;
            }
        }
    }

    void Server::handle_message(const Message &msg)
    {
        switch (msg.type)
        {
        case Message::CONNECT:
        {
            if (m_client_queues.find(msg.sender) != m_client_queues.end())
            {
                std::cerr << "Client name already registered: " << msg.sender << "\n";
                return;
            }

            mqd_t client_queue = mq_open(msg.sender, O_WRONLY);
            if (client_queue == (mqd_t)-1)
            {
                std::cerr << "Error open client queue: " << strerror(errno) << "\n";
                return;
            }
            m_client_queues[msg.sender] = client_queue;
            std::cout << "Client connected to server: " << msg.sender << "\n";

            send_respond(msg.sender, "Connect successfully");
            break;
        }

        case Message::REGISTER_SERVICE:
        {
            std::string method_name = msg.method;
            std::string process_name = msg.sender;

            // std::cout << "Service name: " << service_name << "\n";
            // std::cout << "Process name: " << process_name << "\n";

            if (method_name.empty())
            {
                std::cerr << "Error: Missing method name in REGISTER SERVICE message \n";
                send_respond(process_name, "Error: Missing service or method");
                return;
            }

            // if (m_client_to_service.find(process_name) != m_client_to_service.end())
            // {
            //     std::cerr << "Process " << process_name << " already registered as a service\n";

            //     Message response;
            //     response.type = Message::MessageType::RESPOND;
            //     strncpy(response.sender, "Server", sizeof(response.sender) - 1);
            //     strncpy(response.receiver, process_name.c_str(), sizeof(response.receiver) - 1);
            //     strncpy(response.parameters, "Error: Process already registered as service", sizeof(response.parameters) - 1);

            //     if (mq_send(m_client_queues[process_name], (char *)(&response), sizeof(response), 0) == -1)
            //     {
            //         std::cerr << "Failed to send error response: " << strerror(errno) << "\n";
            //     }
            //     return;
            // }

            auto &methods = m_service_registers[process_name];
            if (std::find(methods.begin(), methods.end(), method_name) != methods.end())
            {
                std::cerr << "Method already registered: " << method_name << " for process " << process_name << "\n";

                send_respond(process_name, "Method already registered");
                return;
            }

            // m_client_to_service[process_name] = service_name;

            methods.push_back(method_name);
            std::cout << "Service registered: " << process_name << " with method " << method_name << "\n";

            send_respond(process_name, "Service registered successfully");
            break;
        }
        case Message::METHOD_CALL:
        {
            std::string caller = msg.sender;
            std::string target_service = msg.receiver;
            std::string target_method = msg.method;

            std::cout << "Received method call from " << caller << " for service " << target_service << " method " << target_method << "\n";
            // std::cout << "Arguments: " << msg.parameters << "\n";

            auto service_it = m_service_registers.find(target_service);
            if (service_it == m_service_registers.end())
            {
                std::cerr << "Service not found: " << target_service << "\n";

                send_respond(caller, "Error: Service not found");
                break;
            }

            auto &methods = service_it->second;
            if (std::find(methods.begin(), methods.end(), target_method) == methods.end())
            {
                std::cerr << "Method not found in service " << target_service << ": " << target_method << "\n";

                send_respond(caller, "Error: Method not found");
                break;
            }

            if (mq_send(m_client_queues[target_service], reinterpret_cast<const char *>(&msg), sizeof(msg), 0) == -1)
            {
                std::cerr << "Failed to forward method call: " << strerror(errno) << "\n";
                send_respond(caller, "Error: Failed to forward method call");
            }
            std::cout << "Method call forwarded to: " << target_service << "\n";
            break;
        }

        case Message::METHOD_RETURN:
            break;

        case Message::RESPOND:
            break;

        case Message::SIGNAL:
        {
            std::string service_name = msg.sender;

            auto it = m_service_subscribers.find(service_name);
            if (it == m_service_subscribers.end())
            {
                std::cerr << "Service not found for SIGNAL: " << service_name << "\n";
                return;
            }

            const auto &subscribers = it->second;
            for (const auto &subscriber : subscribers)
            {
                auto client_queue_it = m_client_queues.find(subscriber);
                if (client_queue_it == m_client_queues.end())
                {
                    std::cerr << "Subscriber queue not found: " << subscriber << "\n";
                    continue;
                }

                if (mq_send(client_queue_it->second, reinterpret_cast<const char *>(&msg), sizeof(msg), 0) == -1)
                {
                    std::cerr << "Failed to send SIGNAL to subscriber: " << subscriber << " - " << strerror(errno) << "\n";
                }
            }

            std::cout << "Signal sent from service " << service_name << " to its subscribers.\n";
            break;
        }

        case Message::SUBSCRIBE:
        {
            std::string service_name = msg.receiver;
            std::string subscriber = msg.sender;
            std::cout << "Received subscribe request from: " << subscriber << "\n";

            auto service = m_client_queues.find(service_name);
            if (service == m_client_queues.end())
            {
                std::cerr << "Service not found: " << service_name << "\n";
                send_respond(subscriber, "Service not found");
                return;
            }

            auto &subscribers = m_service_subscribers[service_name];

            if (std::find(subscribers.begin(), subscribers.end(), subscriber) != subscribers.end())
            {
                std::cout << "Subscriber " << subscriber << " is already subscribed to service " << service_name << ".\n";

                send_respond(subscriber, "Already subscribed");
            }
            else
            {
                subscribers.push_back(subscriber);
                std::cout << "Subscriber " << subscriber << " added to service " << service_name << ".\n";

                send_respond(subscriber, "Successfully subscribed");
            }
            break;
        }

        case Message::UNSUBSCRIBE:
        {
            std::string service_name = msg.receiver;
            std::string subscriber = msg.sender;
            std::cout << "Received unsubscribe request from: " << subscriber << "\n";

            auto it = m_service_subscribers.find(service_name);
            if (it != m_service_subscribers.end())
            {
                auto &subscribers = it->second;
                auto pos = std::find(subscribers.begin(), subscribers.end(), subscriber);
                if (pos != subscribers.end())
                {
                    subscribers.erase(pos);
                    std::cout << "Subscriber " << subscriber << " removed from service " << service_name << ".\n";

                    send_respond(subscriber, "Successfully unsubscribed");
                }
                else
                {
                    std::cout << "Subscriber " << subscriber << " is not subscribed to service " << service_name << ".\n";

                    send_respond(subscriber, "Not subscribed");
                }
            }
            else
            {
                std::cerr << "Service not found: " << service_name << "\n";
                send_respond(subscriber, "Not found service");
            }
            break;
        }

        case Message::DISCONNECT:
            m_client_queues.erase(msg.sender);
            std::cout << "Client disconnected: " << msg.sender << "\n";
            break;

        default:
            std::cout << "Unknow message type received \n";
            break;
        }
    }

    void Server::stop()
    {
        Message shutdown_msg;
        shutdown_msg.type = Message::MessageType::DISCONNECT;
        std::strncpy(shutdown_msg.sender, "Server", sizeof(shutdown_msg.sender) - 1);
        std::strncpy(shutdown_msg.parameters, "Server shutdown", sizeof(shutdown_msg.parameters) - 1);

        for (const auto &client_queue : m_client_queues)
        {
            if (mq_send(client_queue.second, reinterpret_cast<const char *>(&shutdown_msg), sizeof(shutdown_msg), 0) == -1)
            {
                std::cerr << "Failed to send notify to clients: " << client_queue.first << "\n";
            }
            mq_close(client_queue.second);
        }
        m_client_queues.clear();
        if (m_server_queue != -1)
        {
            mq_close(m_server_queue);
            mq_unlink("/server_test");
            m_server_queue = -1;
        }

        std::cout << "Server stopped \n";
    }

    void Server::send_respond(const std::string &receiver, const std::string &message)
    {
        Message response;
        response.type = Message::MessageType::RESPOND;
        strncpy(response.sender, "Server", sizeof(response.sender) - 1);
        response.sender[sizeof(response.sender) - 1] = '\0';

        strncpy(response.receiver, receiver.c_str(), sizeof(response.receiver) - 1);
        response.receiver[sizeof(response.receiver) - 1] = '\0';

        strncpy(response.parameters, message.c_str(), sizeof(response.parameters) - 1);
        response.parameters[sizeof(response.parameters) - 1] = '\0';

        auto queue_it = m_client_queues.find(receiver);
        if (queue_it == m_client_queues.end())
        {
            std::cerr << "Failed to find queue for receiver: " << receiver << "\n";
            return;
        }

        if (mq_send(queue_it->second, reinterpret_cast<const char *>(&response), sizeof(response), 0) == -1)
        {
            std::cerr << "Failed to send response to " << receiver << ": " << strerror(errno) << "\n";
        }
    }
}