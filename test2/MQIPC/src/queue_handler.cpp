#include <ctime>
#include <cstring>
#include "queue_handler.h"

MQIPC::MQueueHandler::MQueueHandler(
    const std::string& name,
    int flags, mode_t mode,
    const mq_attr* attr,
    bool should_unlink)
    : _mqueue((mqd_t)-1)
    , _queue_name(name)
    , _should_unlink(should_unlink)
{
    if (attr) {
        _mqueue = mq_open(name.c_str(), flags, mode, attr);
    }
    else {
        _mqueue = mq_open(name.c_str(), flags);
    }

    if (_mqueue == (mqd_t)-1) {
        throw std::runtime_error("Failed to open message queue: " +
            std::string(strerror(errno)));
    }
}

MQIPC::MQueueHandler::~MQueueHandler()
{
    clear();
}

void MQIPC::MQueueHandler::clear()
{
    if (is_valid()) {
        mq_close(_mqueue);
        if (_should_unlink) {
            mq_unlink(_queue_name.c_str());
        }
        _mqueue = (mqd_t)-1;
    }
}

timespec MQIPC::MQueueHandler::ms_to_timespec(unsigned long milliseconds)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += milliseconds / 1'000;
    ts.tv_nsec += (milliseconds % 1'000) * 1'000'000;

    if (ts.tv_nsec >= 1'000'000'000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1'000'000'000;
    }
    return ts;
}

MQIPC::MQueueHandler::MQueueHandler(MQueueHandler&& other) noexcept
    : _mqueue(other._mqueue)
    , _queue_name(std::move(other._queue_name))
    , _should_unlink(other._should_unlink)
{
    other._mqueue = (mqd_t)-1;
    other._should_unlink = false;
}

MQIPC::MQueueHandler& MQIPC::MQueueHandler::operator=(MQueueHandler&& other) noexcept
{
    if (this != &other)
    {
        clear();
        _mqueue = other._mqueue;
        _queue_name = std::move(other._queue_name);
        _should_unlink = other._should_unlink;

        other._mqueue = (mqd_t)-1;
        other._should_unlink = false;
    }
    return *this;
}

mqd_t MQIPC::MQueueHandler::get() const
{
    return _mqueue;
}

bool MQIPC::MQueueHandler::is_valid() const
{
    return _mqueue != (mqd_t)-1;
}

bool MQIPC::MQueueHandler::get_attributes(mq_attr* attr) const
{
    return mq_getattr(_mqueue, attr) != -1;
}

bool MQIPC::MQueueHandler::set_attributes(const mq_attr* attr)
{
    return mq_setattr(_mqueue, attr, nullptr) != -1;
}

std::string MQIPC::MQueueHandler::get_last_error() const
{
    return std::string(strerror(errno));
}

int MQIPC::MQueueHandler::get_errno() const
{
    return errno;
}

MQIPC::MQueueHandler::SendResult MQIPC::MQueueHandler::send(const char* msg_ptr, size_t msg_len, uint msg_prio)
{
    if (mq_send(_mqueue, msg_ptr, msg_len, msg_prio) == -1)
    {
        return SendResult::Error;
    }
    return SendResult::Success;
}

MQIPC::MQueueHandler::ReceiveResult MQIPC::MQueueHandler::receive(char* msg_ptr, size_t msg_len, uint msg_prio)
{
    if (mq_receive(_mqueue, msg_ptr, msg_len, &msg_prio) == -1)
    {
        return ReceiveResult::Error;
    }
    return ReceiveResult::Success;
}

MQIPC::MQueueHandler::SendResult MQIPC::MQueueHandler::timed_send(const char* msg_ptr, size_t msg_len, unsigned int msg_prio, unsigned long timeout_ms)
{
    timespec abs_timeout = ms_to_timespec(timeout_ms);

    int result = mq_timedsend(_mqueue, msg_ptr, msg_len,
        msg_prio, &abs_timeout);

    if (result >= 0) {
        return SendResult::Success;
    }

    return (errno == ETIMEDOUT) ? SendResult::Timeout : SendResult::Error;
}

MQIPC::MQueueHandler::ReceiveResult MQIPC::MQueueHandler::timed_receive(char* msg_ptr, size_t msg_len, unsigned int msg_prio, unsigned long timeout_ms)
{
    timespec abs_timeout = ms_to_timespec(timeout_ms);

    int result = mq_timedreceive(_mqueue, msg_ptr, msg_len, &msg_prio, &abs_timeout);

    if (result >= 0)
    {
        return ReceiveResult::Success;
    }

    return (errno == ETIMEDOUT) ? ReceiveResult::Timeout : ReceiveResult::Error;
}


