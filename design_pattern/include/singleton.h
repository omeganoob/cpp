#pragma once

#include <string>
#include <mutex>
#include <cassert>

#ifndef SINGLETON__
#define SINGLETON__

class Singleton
{
protected:
    Singleton(std::string name) : _name{ name }
    {
    }
    static Singleton* _instance;
    static std::mutex _mutex;
    std::string _name;
public:

    Singleton(Singleton&) = delete;
    void operator=(const Singleton&) = delete;

    static Singleton& Instance(std::string name);

    void PrintName() const;
};

class NotSingleton
{
public:
    NotSingleton()
    {
        assert(!_created);
        _created = true;
    }

private:
    static bool _created;
};

bool NotSingleton::_created = false;

#endif