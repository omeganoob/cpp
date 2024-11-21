#include <cstddef>
#include <list>
#include <string>
#include <map>
enum Event
{
    Attack,
    Jump,
    Run,
    Hurt
};

class IObserver
{
public:
    virtual ~IObserver() {}
    virtual void onNotify(Event event, std::string msg) = 0;
};

class ISubject
{
public:
    virtual ~ISubject() {}
    virtual void addObserver(IObserver* observer) = 0;
    virtual void addObserver(IObserver* observer, Event e) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    static const size_t MAX_OBSERVERS{ 10 };
protected:
    std::list<IObserver*> _observers;
    std::map<Event, std::list<IObserver*>> _listenerMap;
    int _numObservers;
    virtual void notify(Event event, std::string msg) = 0;
};