#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include "singleton.h"

auto thread_deleter = [](std::thread* thrd)
{
    if(thrd->joinable())
    {
        thrd->join();
    }
};

using auto_join_thread = std::unique_ptr<std::thread, decltype(thread_deleter)>;

void ThreadFoo()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton& singleton = Singleton::Instance("HEHEHEHEEH");
    singleton.PrintName();
}

void ThreadBar()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton& singleton = Singleton::Instance("aaaaaaaaaaaaaaaa");
    singleton.PrintName();
}

int main(int, char**){

    auto_join_thread t1(new std::thread(ThreadFoo), thread_deleter);
    auto_join_thread t2(new std::thread(ThreadBar), thread_deleter);
    
    return 0;
}
