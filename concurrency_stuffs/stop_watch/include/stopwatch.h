#include <chrono>
class Stopwatch
{
public:
    Stopwatch(std::chrono::nanoseconds& result);
    ~Stopwatch();
private:
    std::chrono::nanoseconds& _result;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
};