#include <iostream>
#include <queue>
#include <deque>
#include <map>
#include <utility>

int main(int, char**)
{
    std::map<int, double> m
    {
        {1, 2.2},
    };
    m.insert({2, 6.7});
    m[2] = 3.14;
    std::cout << m.size() << "\n";
    for (auto& [A, B] : m)
    {
        std::cout << A << ": " << B << "\n";
    }
    return 0;
}