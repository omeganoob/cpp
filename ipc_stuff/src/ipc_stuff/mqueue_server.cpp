#include <iostream>
#include <error.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>

constexpr char* MQUEUE_NAME = "/my_mqueue";
constexpr int MQUEUE_PER = 0644;
constexpr long MQUEUE_MAX_MSG = 10;
constexpr long MQUEUE_MSG_LENGTH = 1024;
constexpr int QUEUE_MAX_PRIO = 9;

int main(int argc, char** argv)
{
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MQUEUE_MAX_MSG;
    attr.mq_msgsize = MQUEUE_MSG_LENGTH;
    attr.mq_curmsgs = 0;

    mqd_t mq_descriptor;
    uint prio = 0;

    std::string in_msg;

    mq_descriptor = mq_open(MQUEUE_NAME, O_CREAT | O_WRONLY , MQUEUE_PER, &attr);
    
    if (mq_descriptor == (mqd_t)-1)
    {
        perror("mq_open()");
        return 1;
    }

    while (true)
    {
        std::cout << "Enter message: ";
        
        std::getline(std::cin, in_msg);
        
        if (in_msg == "end")
        {
            break;
        }
        if (mq_send(mq_descriptor, in_msg.c_str(), in_msg.length(), prio) == -1)
        {
            perror("mq_send()");
        }
    }

    if (mq_send(mq_descriptor, in_msg.c_str(), in_msg.length(), prio) == -1)
    {
        perror("mq_send()");
    }
    mq_close(mq_descriptor);
    mq_unlink(MQUEUE_NAME);
    return 0;
}
