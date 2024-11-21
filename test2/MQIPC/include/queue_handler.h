#include <mqueue.h>
#include <string>
#include <stdexcept>
#include <vector>

namespace MQIPC
{
    class MQueueHandler
    {
    private:
        mqd_t _mqueue;
        std::string _queue_name;
        bool _should_unlink;

        static timespec ms_to_timespec(unsigned long milliseconds);
    public:
        MQueueHandler(
            const std::string& name,
            int flags,
            mode_t mode = 0,
            const mq_attr* attr = nullptr,
            bool should_unlink = false);
        ~MQueueHandler();

        void clear();

        MQueueHandler(const MQueueHandler&) = delete;
        MQueueHandler& operator=(const MQueueHandler&) = delete;
        MQueueHandler(MQueueHandler&& other) noexcept;
        MQueueHandler& operator=(MQueueHandler&& other) noexcept;

        mqd_t get() const;
        bool is_valid() const;
        bool get_attributes(mq_attr* attr) const;
        bool set_attributes(const mq_attr* attr);
        std::string get_last_error() const;
        int get_errno() const;

        enum class SendResult {
            Success,
            Timeout,
            Error
        };

        enum class ReceiveResult {
            Success,
            Timeout,
            Error
        };

        SendResult send(const char* msg_ptr, size_t msg_len, uint msg_prio);
        ReceiveResult receive(char* msg_ptr, size_t msg_len, uint msg_prio);

        SendResult timed_send(
            const char* msg_ptr,
            size_t msg_len,
            unsigned int msg_prio,
            unsigned long timeout_ms);

        ReceiveResult timed_receive(
            char* msg_ptr,
            size_t msg_len,
            unsigned int msg_prio,
            unsigned long timeout_ms);

        template<typename T>
        ReceiveResult timed_receive_struct(T& msg, unsigned int* priority,
            unsigned long timeout_ms);

    };
}