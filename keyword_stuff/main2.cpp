#include <iostream>

int main(int, char**)
{   
    // volatile constexpr int val = 10;
    constexpr int val = 10;

    int* ptr = const_cast<int*>(&val);

    std::cout << &val << "\n" << ptr << "\n";

    *ptr = 88;

    std::cout << "-------------------\n";

    std::cout << val << "\n" << *ptr << "\n";

    return 0;
}