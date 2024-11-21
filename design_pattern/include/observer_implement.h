#include <iostream>
#include "observer.h"

class AttackListener : public IObserver
{
public:
    void onNotify(Event e, std::string msg) override;
    virtual ~AttackListener() override {}
};

class JumpListener : public IObserver
{
public:
    void onNotify(Event, std::string) override;
    virtual ~JumpListener() override {}
};

class Player : public ISubject
{
public:
    void addObserver(IObserver* observer) override;
    void addObserver(IObserver* observer, Event e) override;
    void removeObserver(IObserver* observer) override;
    void notify(Event e, std::string msg) override;
    virtual ~Player() override {}
};