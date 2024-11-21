#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <cstdio>
#include <string_view>
#include <iostream>

constexpr char* MQUEUE_NAME = "/my_mqueue";
constexpr int MQUEUE_PER = 0644;
constexpr long MQUEUE_MAX_MSG = 10;
constexpr long MQUEUE_MSG_LENGTH = 1024;
constexpr int QUEUE_MAX_PRIO = 9;

#define NOTIFY_SIG SIGUSR1

static void handler(int sig)
{
    std::cout << "Received signal: " << sig << ".\n";
}

int main(int argc, char** argv)
{

    struct sigevent sev;

    mqd_t mq_descriptor;

    struct mq_attr attr;

    std::vector<char> buffer(MQUEUE_MSG_LENGTH);

    sigset_t blockMask, emptyMask;

    struct sigaction sa;

    mq_descriptor = mq_open(MQUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK);
    if (mq_descriptor == (mqd_t)-1)
    {
        perror("mq_open()");
    }

    if (mq_getattr(mq_descriptor, &attr) == -1)
    {
        perror("mq_getattr()");
    }

    sigemptyset(&blockMask);
    
    sigaddset(&blockMask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1)
    {
        perror("sigprocmask()");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1)
    {
        perror("sigaction()");
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mq_descriptor, &sev) == -1)
    {
        perror("mq_notify()");
    }

    sigemptyset(&emptyMask);

    ssize_t byte_read;

    while (true)
    {
        sigsuspend(&emptyMask);

        if (mq_notify(mq_descriptor, &sev) == -1)
        {
            perror("mq_notify()");
        }

        while((byte_read = mq_receive(mq_descriptor, buffer.data(), buffer.size(), NULL)) >= 0)
        {
            std::string_view str_v(buffer.data(), byte_read);
            std::cout << "Message received: " << str_v << "\n";
            if(str_v == "end")
            {
                break;
            }
        }

        if(errno != EAGAIN)
        {
            perror("mq_receive()");
            break;
        }
    }

    mq_close(mq_descriptor);
    return 0;
}