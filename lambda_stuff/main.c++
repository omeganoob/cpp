#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

//function objects - functor
// f();

class Shape
{
public:
    int width{}, height{};
    int* data;
    Shape()
    {
        data = new int[100000];
        data[99999] = 100;
        std::cout << "width address: " << &width << "\n";
        std::cout << "height address: " << &height << "\n";
    }

    std::function<int()> get_lambda()
    {
        return [this]() -> int {
            std::cout << "lambda - data[9999]: " << data[99999] << "\n";
            std::cout << "lambda - width address: " << &width << "\n";
            std::cout << "lambda - height address: " << &height << "\n";
            return width + height;
        };
    }
    ~Shape()
    {
        delete[] data;
        std::cout << "Shape destroyed!\n";
    }
};

struct FuncHehe
{
    int m_x{ 2 };
    int m_y{ 3 };

    FuncHehe(int x, int y)
        : m_x{ x }
        , m_y{ y }
    {
    }

    void operator() ()
    {
        std::cout << "hehe\n";
    }

    int operator() (int a, int b)
    {
        return a + b + (m_x * m_y);
    }
};

int foo(int a, int b)
{
    std::cout << "foo called.\n";
    return a + b;
}

void MyFunc(int a, int b, std::function<int(int, int)> func)
{
    std::cout << "MyFunc call: func(" << a << ", " << b << ")\n" << func(a, b) << "\n";
}

template<typename Type, typename Function>
void bar(Type a, Type b, Function func)
{
    std::cout << "template lambda: " << func(a, b) << "\n";
}

// void bar(std::string a, std::string b, std::function<std::string(std::string, std::string)> func)
// {
    
// }

// void bar(int a, int b, std::function<int(int, int)> func)
// {
    
// }

int main(int, char**)
{
    FuncHehe fhehe{ 1, 2 };

    // int(*fooptr)(int);

    int x = 3;
    std::cout << "x address main: " << &x << "\n";
    int x1 = 4;
    int x2 = 5;
    int x3 = 6;
    int x4 = 7;

    auto f = [&](int a, int b) {
        std::cout << "x address lambda: " << &x << "\n";
        return a + b + x;
        };
    f(1, 2);

    std::cout << fhehe(1, 3) << "\n";

    MyFunc(1, 2, foo);
    MyFunc(2, 3, f);
    MyFunc(3, 3, [](int a, int b) { return a * b; });

    auto vjplambda = [](auto a, auto b) {return a + b;};

    bar(1, 2, vjplambda);
    bar(std::string("chung"), std::string(" nguyen"), vjplambda);
    
    // MyFunc(3, 4, fhehe);

    std::function<int()> slambda;

    {
        Shape sh;
        slambda = sh.get_lambda();
        sh.width = 5;
        sh.height = 6;
        std::cout << "call shape lambda in scope: " << slambda() << "\n";
    }

    // std::cout << "call shape lambda in main: " << slambda() << "\n";

    return 0;
}