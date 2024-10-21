// #include "my_share.h"
#include <iostream>
#include <memory>
#include <utility>

template<typename T>
class MyPointer
{
    T* m_ptr;
public:
    MyPointer(T* ptr = nullptr)
        :m_ptr(ptr)
    {
        std::cout << "pointer created.\n";
    }
    ~MyPointer()
    {
        delete m_ptr;
        std::cout << "pointer destroyed.\n";
    }

    MyPointer(MyPointer& p)
        :m_ptr{ p.m_ptr }
    {
        p.m_ptr = nullptr;
    }

    MyPointer& operator=(MyPointer& p)
    {
        if (this == &p)
            return *this;

        delete m_ptr;
        m_ptr = p.m_ptr;
        p.m_ptr = nullptr;

        return *this;
    }

    MyPointer(MyPointer&& p)
        :m_ptr{ p.m_ptr }
    {
        p.m_ptr = nullptr;
    }

    MyPointer& operator=(MyPointer&& p)
    {
        if (this == &p)
            return *this;

        delete m_ptr;
        m_ptr = p.m_ptr;
        p.m_ptr = nullptr;

        return *this;
    }

    T& operator*() const
    {
        return *m_ptr;
    }
    T* operator->() const
    {
        return m_ptr;
    }
};

template<typename Unit, typename Color>
class Pixel
{
    Unit m_x;
    Unit m_y;
    Color m_color;
public:
    Pixel(Unit x, Unit y, Color c)
        : m_x(x)
        , m_y(y)
        , m_color(c)
    {
        std::cout << "A new Pixel allocated.\n";
    }
    ~Pixel()
    {
        std::cout << "A Pixel freed.\n";
    }

    const void print() const
    {
        std::cout << "Pixel: " << "x: " << m_x << " - y: " << m_y << " - color: " << m_color << "\n";
    }
};

using  uni_pixelPtr = std::unique_ptr<Pixel<int, std::string>>;

/*
void foo(int a)
{
    // Point<int, std::string>* p = new Point<int, std::string>(0, 0, "#ff0000");

    MyPointer<Point<int, std::string>> pPtr = new Point<int, std::string>{0, 0, "#ff0000"};

    if (a < 0)
    {
        return;
    }

    // delete p;
}
*/

const uni_pixelPtr& foo(const uni_pixelPtr& p)
{
    p->print();
    return p;
}

int main(int, char**) {

    MyPointer<Pixel<int, std::string>> pPtr
        = new Pixel<int, std::string>{ 0, 0, "#ff0000" };

    std::unique_ptr<Pixel<int, std::string>> pUptr
        = std::make_unique<Pixel<int, std::string>>(0, 0, "#00ff00");

    // uni_pixelPtr p2 = std::move(pUptr);

    foo(pUptr);

    if (pUptr.get() == nullptr)
    {
        std::cout << "NO" << "\n";
    }
    else
    {
        std::cout << "YES" << "\n";
    }

    std::shared_ptr<Pixel<int, std::string>> pSprt
        = std::make_shared<Pixel<int, std::string>>(1, 1, "#0000ff");

    return 0;
}
