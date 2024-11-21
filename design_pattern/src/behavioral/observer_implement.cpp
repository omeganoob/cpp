#include "observer_implement.h"

void AttackListener::onNotify(Event e, std::string msg)
{
    if (e == Event::Attack)
    {
        std::cout << "AttackListener: " << msg << "\n";
    }
}

void JumpListener::onNotify(Event e, std::string msg)
{
    if (e == Event::Jump)
    {
        std::cout << "JumpListener: " << msg << "\n";
    }
}

void Player::addObserver(IObserver* observer)
{
    _observers.push_back(observer);
}

void Player::addObserver(IObserver* observer, Event e)
{
    _listenerMap[e].push_back(observer);
}

void Player::removeObserver(IObserver* observer)
{
    _observers.remove(observer);
}

void Player::notify(Event e, std::string msg)
{
    for (auto&& i : _observers)
    {
        i->onNotify(e, msg);
    }
}





