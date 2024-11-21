#pragma once

#include <iostream>
namespace KIPC
{
    struct Message
    {
        enum MessageType
        {
            CONNECT,
            REGISTER_SERVICE,
            SUBSCRIBE,
            UNSUBSCRIBE,
            METHOD_CALL,
            METHOD_RETURN,
            RESPOND,
            DISCONNECT,
            SIGNAL,
        };

        MessageType type;
        char sender[32];
        char receiver[32];
        // char service[32];
        char method[32];
        char parameters[100];
        // char relation_id[100];
    };

    void printMessage(const Message &msg);
    struct Signal
    {
        // something
    };

    struct Error
    {
        // something
    };
}