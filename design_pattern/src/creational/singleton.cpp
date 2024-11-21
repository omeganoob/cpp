#include <iostream>
#include "singleton.h"

Singleton* Singleton::_instance{ nullptr };
std::mutex Singleton::_mutex;

Singleton& Singleton::Instance(std::string name)
{
    if (_instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_instance == nullptr)
        {
            _instance = new Singleton(name);
        }
    }
    return *_instance;
}

void Singleton::PrintName() const
{
    std::cout << _name << ".\n";
}