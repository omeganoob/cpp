#include "stopwatch.h"

Stopwatch::Stopwatch(std::chrono::nanoseconds& result)
    : _result(result)
    , _start(std::chrono::high_resolution_clock::now())
{
}

Stopwatch::~Stopwatch()
{
    _result = std::chrono::high_resolution_clock::now() - _start;
}


