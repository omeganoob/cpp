#include <thread>
#include <iostream>
#include <utility>

class scoped_thread
{
public:
    explicit scoped_thread(std::thread t_)
        : _t(std::move(t_))
    {
        if (!_t.joinable())
            throw std::logic_error("No joinable thread!");
    }
    ~scoped_thread()
    {
        _t.join();
    }
    scoped_thread(scoped_thread&) = delete;
    scoped_thread& operator= (scoped_thread const&) = delete;
private:
    std::thread _t;
};
struct func
{
    int& i;
    func(int& i_) :i(i_) {}
    void operator()()
    {
        for (unsigned j = 0;j < 1000000;++j)
        {
            std::cout << i+j << "\n";
        }
    }
};

void foo()
{
    int some_local_state{};
    scoped_thread t{ std::thread(func(some_local_state))};
    //do something in current thread
}

int main(int, char**)
{

    return 0;
}