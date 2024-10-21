#include <iostream>
#include <string>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

class Widget
{
private:
    int m_i{ 0 };
    std::string m_s{};
    int* m_pi{};

public:
    
    Widget(int i = 0, std::string s = "", int* pi = nullptr)
        : m_i{ i }
        , m_s{ s }
        , m_pi{ pi }
    {
        std::cout << "normal constructor " << s << "\n";
    }

    //copy constructor
    Widget(const Widget& w) 
    noexcept
        : m_i(w.m_i)
        , m_s(w.m_s)
    {
        std::cout << "copy constructor called\n";
        m_pi = new int{};
        *m_pi = *(w.m_pi);
    }

    //copy assignment
    Widget& operator=(const Widget& w)
    {
        if(this == &w)
            return *this;
        std::cout << "copy ass\n";
        m_i = w.m_i;
        m_s = w.m_s;

        delete m_pi;
        m_pi = new int{};
        if(w.m_pi)
        {
            *m_pi = *(w.m_pi);
        }
        else {
            m_pi = nullptr;
        }
        return *this;
    }

    //move constructor
    Widget(Widget&& w) noexcept
        : m_i(std::move(w.m_i))
        , m_s(std::move(w.m_s))
        , m_pi(std::move(w.m_pi))
    {
        std::cout << "move constructor called\n";
        w.m_pi = nullptr;
        w.m_i = 0;
    }
    
    //move assignment 
    Widget& operator=(Widget&& w)
    {

        assertm(this != &w,"Move to self");

        std::cout << "move ass\n";
        
        delete m_pi;

        m_i = std::move(w.m_i);
        m_s = std::move(w.m_s);
        m_pi = std::move(w.m_pi);
        
        w.m_i = 0;
        w.m_pi = nullptr;
        
        return *this;
    }

    void print()
    {
        std::cout << "\n----------------------------------------------\n";
        std::cout << "i: " << m_i << "\n";
        std::cout << "s: " << m_s << "\n";
        std::cout << "pi: " << m_pi << ", with value: " << getValPi() << "\n";
        std::cout << "\n----------------------------------------------\n";
    }

    const int getValPi() const
    {
        if(m_pi)
        {
            return *m_pi;
        }
        return 0;
    }
    
    void SetStr(std::string& s)
    {
        m_s = std::move(s);
    }

    ~Widget()
    {
        std::cout << "destroyed.\n";
        delete m_pi;
    }
};

Widget generateWidget()
{
    return Widget { 99, "w" , new int{88} };
}

void SwapW(Widget& a, Widget& b)
{
    Widget _temp = std::move(a);
    a = std::move(b);
    b = std::move(_temp);
    _temp.print();
}

int main(int, char**) {
    
    // Widget w1{99, "w1", new int{97}};
    // Widget w2;

    // w1.print();
    // w2.print();

    // w2 = std::move(w1);

    // w1.print();
    // w2.print();

    Widget w {123, "my w", new int{77}};
    // Widget w1;
    // w1 = generateWidget();
    // w1 = w;
    
    return 0;
}
