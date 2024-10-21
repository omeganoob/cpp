#include <iostream>
#include <string>
#include <algorithm>

__int128_t get_fibo(uint nth)
{
    if(nth == 0 || nth == 1)
        return 0;

    __int128_t fib, fib1 = 0, fib2 = 1;
    for(uint i = 2; i <= nth; i++)
    {
        fib = fib1 + fib2;
        fib1 = fib2;
        fib2 = fib;
    }

    return fib;
}

void print_huge_num(__int128_t num)
{
    if(num == 0)
    {
        std::cout << (int)num << "\n";
        return;
    }
    std::string num_str{};
    while (num > 0)
    {
        num_str.push_back(48 + num%10);
        num = num / 10;
    }

    std::reverse(num_str.begin(), num_str.end());
    std::cout << num_str << "\n";
}

int main(int argc, char** argv)
{   
    uint nth;
    while(std::cin >> nth && nth <=100)
    {
        std::cout << nth << "-th fibonanci number is: ";
        print_huge_num(get_fibo(nth));
    }    
    return 0;
}