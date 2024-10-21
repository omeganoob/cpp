#include "stopwatch.h"
#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <set>
#include <chrono>
#include <sstream>
#include <iostream>
#include <limits>
#include <vector>

template <typename T>
std::set<T> factorize(T x)
{
    std::set<T> result{ 1 };
    for (T candidate{ 2 }; candidate <= x; candidate++)
    {
        if (x % candidate == 0)
        {
            result.insert(candidate);
            x /= candidate;
            candidate = 1;
        }
    }

    return result;
}

std::string factor_task(unsigned long x)
{
    std::chrono::nanoseconds elapsed_ns;
    std::set<unsigned long long> factors;
    {
        Stopwatch stop_watch{ elapsed_ns };
        factors = factorize<unsigned long long>(x);
    }
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_ns).count();
    std::stringstream ss;
    ss << elapsed_ms << " ms: Factoring " << x << " ( ";
    for (auto factor : factors)
    {
        ss << factor << " ";
    }
    ss << ")\n";

    return ss.str();
}

std::vector<unsigned long long> numbers
{
    9'699'690,
    179'426'549,
    1'000'000'007,
    4'294'967'291,
    4'294'967'296,
    1'307'674'368'000
};

int main(int, char**)
{
    /*Normal version, total time equal to sum of all task.*/
    // std::chrono::nanoseconds elapsed_ns;
    // {
    //     Stopwatch stop_watch{ elapsed_ns };
    //     for(const auto& number: numbers)
    //     {
    //         std::cout << factor_task(number);
    //     }
    // }
    // const auto enlapsed_ms = 
    //             std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_ns).count();
    // std::cout << enlapsed_ms << "ms: total program time\n";

    /*Async version, total time roughly the same as the longest task.*/
    std::chrono::nanoseconds elapsed_ns;
    {
        Stopwatch stop_watch{ elapsed_ns };
        std::vector<std::future<std::string>> factor_tasks;
        for (const auto& number : numbers)
        {
            factor_tasks.emplace_back(std::async(std::launch::async, factor_task, number));
        }
        for(auto& task: factor_tasks)
        {
            std::cout << task.get();
        }
    }
    const auto enlapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_ns).count();
    std::cout << enlapsed_ms << "ms: total program time\n";
    return 0;
}
