#include <iostream>
#include <error.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <string_view>
#include <thread>

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


    mq_descriptor = mq_open(MQUEUE_NAME, O_CREAT | O_RDONLY, MQUEUE_PER, &attr);
    if (mq_descriptor == (mqd_t)-1)
    {
        perror("mq_open()");
        return 1;
    }

    if (mq_getattr(mq_descriptor, &attr) == -1)
    {
        perror("mq_getattr()");
        mq_close(mq_descriptor);
        return 1;
    }

    std::vector<char> read_buffer(MQUEUE_MSG_LENGTH);
    
    while (true)
    {

        // if (mq_getattr(mq_descriptor, &attr) == -1)
        // {
        //     continue;
        // }
        // if (attr.mq_curmsgs != 0)
        // {
        //     ssize_t bytes_reads = mq_receive(mq_descriptor, read_buffer.data(), read_buffer.size(), &prio);
        //     if (bytes_reads == -1)
        //     {
        //         perror("mq_receive()");
        //         break;
        //     }
        //     else
        //     {
        //         std::string_view str_v(read_buffer.data(), bytes_reads);
        //         std::cout << "Message received: " << str_v << "\n";
        //         if (str_v == "end")
        //         {
        //             break;
        //         }
        //     }
        // }
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        ssize_t bytes_reads = mq_receive(mq_descriptor, read_buffer.data(), read_buffer.size(), &prio);
            if (bytes_reads == -1)
            {
                perror("mq_receive()");
                break;
            }
            else
            {
                std::string_view str_v(read_buffer.data(), bytes_reads);
                std::cout << "Message received: " << str_v << "\n";
                if (str_v == "end")
                {
                    break;
                }
            }
    }
    mq_close(mq_descriptor);
    return 0;
}