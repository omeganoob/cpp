#include "stopwatch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <tuple>

std::mutex coutMutex;

struct ThreadGuard
{
    ThreadGuard(std::thread& thread) 
        : _thread{ thread }
    {}
    ~ThreadGuard()
    {
        if(_thread.joinable())
        {
            _thread.join();
        }
    }
    ThreadGuard(ThreadGuard const&) = delete;
    ThreadGuard& operator=(ThreadGuard const&) = delete;    
private:
    std::thread& _thread;
};

// TODO: turn to template
struct GuardedThread
{
    GuardedThread(std::function<void(const uint&, const std::string&)> work, const uint& x, const std::string& name)
        :_thread(work, x, name)
        ,_thread_guard(_thread)
    {}

    GuardedThread(GuardedThread&) = delete;
    GuardedThread& operator=(GuardedThread&) = delete;
    
    bool detach()
    {
        if(!_thread.joinable())
        {
            _thread.detach();
            return true;
        }
        return false;
    }
    bool join()
    {
        if(_thread.joinable())
        {
            _thread.join();
            return true;
        }
        return false;
    }
private:
    std::thread _thread;
    ThreadGuard _thread_guard;
};

void print(const uint& x, const std::string& name)
{
    for (size_t i = 0; i <= x; i++)
    {
        std::cout << name << ": " << i << "\n";
    }
}

int main(int, char**)
{
    // std::thread t1(print, 10, "thread 1");
    // std::thread t2(print, 10, "thread 2");
    // std::thread t3([](const uint x = 10, const std::string name = "thread 3") {
    //     for (size_t i = 0; i <= x; i++)
    //     {
    //         std::cout << name << ": " << i << "\n";
    //     }
    // });

    // std::thread(print, 10, "thread 1").join();
    // std::thread(print, 10, "thread 2").join();

    GuardedThread gt{print, 10, "guarded thread"};

    std::cout << (gt.join() ? "joined" : "can't join") << "\n";
    
    std::cout << "main end\n";
    
    return 0;
}