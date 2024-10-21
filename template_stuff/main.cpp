// #include "my_template.h"
#include <iostream>
#include <vector>
#include <cassert>

// template<typename T1, typename T2>
// T1 max(T1 a, T2 b)
// {
//     return a > b ? a : b;
// }

// typedef struct Rect
// {
//     int x, y, w, h;
//     bool operator>(const Rect& rect)
//     {
//         return w * h > rect.w * rect.h;
//     }
//     operator int() const
//     {
//         return w * h;
//     }
// } Rect;

// template <typename M, typename U>
// class Point
// {
// public:
//     Point(M color, U x, U y)
//         : m_color{ color }
//         , m_x{ x }
//         , m_y{ y }
//     {

//     }
//     M getColor() const
//     {
//         return m_color;
//     }
//     void setColor(const M& color)
//     {
//         m_color = color;
//     }
//     U getX() const
//     {
//         return m_x;
//     }
//     void setX(const U& x)
//     {
//         m_x = x;
//     }
//     U getY() const
//     {
//         return m_y;
//     }
//     void setY(const U& y)
//     {
//         m_y = y;
//     }
// private:
//     M m_color;
//     U m_x, m_y;
// };

// std::ostream& operator<<(std::ostream& os, Rect& rect)
// {
//     os << "w: " << rect.w << " /" << " h: " << rect.h << "\n";
//     return os;
// }

class Fraction
{
public:
    explicit Fraction(int a = 0, int b = 1)
        : m_a{ a }
        , m_b{ b > 0 ? b : 1 }
    {
        std::cout << "[Fraction] Normal constructor.\n";
    }

    // Fraction(std::initializer_list<int> il)
    // {
    //     std::cout << "[Fraction] List init constructor.\n";
    // }

    Fraction(Fraction& f)
        : m_a{ f.m_a }
        , m_b{ f.m_b }
    {
        std::cout << "[Fraction] Copy constructor.\n";
    }
    Fraction& operator=(const Fraction& f)
    {
        if (this == &f)
            return *this;
        std::cout << "[Fraction] Copy assignment.\n";
        m_a = f.m_a;
        m_b = f.m_b;
        return *this;
    }

    Fraction(Fraction&& f)
        : m_a{ f.m_a }
        , m_b{ f.m_b }
    {
        std::cout << "[Fraction] Move constructor.\n";
        f.m_a = 0;
        f.m_b = 1;
    }
    Fraction& operator=(Fraction&& f)
    {
        if (this == &f)
            return *this;

        std::cout << "[Fraction] Move assignment.\n";

        m_a = f.m_a;
        m_b = f.m_b;
        f.m_a = 0;
        f.m_b = 1;

        return *this;
    }
    void print() const
    {
        std::cout << m_a << "/" << m_b << "\n";
    }
private:
    int m_a{};
    int m_b{ 1 };
};

class MyArray
{
public:
    MyArray() = default;
    MyArray(std::initializer_list<int> il)
        : m_sz{ il.size() }
        , m_ptr{ new int[il.size()] }
    {
        int idx = 0;
        for (auto&& i : il)
        {
            m_ptr[idx] = i;
            idx++;
        }
        // std::copy(il.begin(), il.end(), m_ptr);
    }
    int operator[](uint idx)
    {
        assert(idx < m_sz && "Index out of range!");
        return m_ptr[idx];
    }
    ~MyArray()
    {
        delete[] m_ptr;
    }
private:
    size_t m_sz{};
    int* m_ptr{ nullptr };
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    for (auto&& i : vec)
    {
        os << i << " ";
    }
    os << std::endl;
    return os;
}

template<typename T, typename U>
struct MyPair
{
    T first;
    U second;
};

template<typename V>
struct MyPair<int, V>
{
    int first;
    V second;
};




int main(int, char**)
{
    
    return 0;
}