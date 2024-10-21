#include <iostream>
#include <type_traits>
#include <utility>
#include <concepts>

template<typename T>
concept Addable = requires (T a, T b)
{
    a + b;
};

class NoAdd {};

template<Addable A>
A add(const A& a, const A& b)
{
    return a + b;
}

int main() {

    NoAdd a;
    NoAdd b;

    // add(a, b);

    long c = 10000;
    long d = 99999;

    add(c, d);
    add(1.2, 3.5);
    add(std::string{"chung"}, std::string{"nguyen"});
    return 0;
}