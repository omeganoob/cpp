#include <iostream>
#include <iomanip>

#define ADD 1
#define SUB 2
#define MUL 4
#define DIV 8
#define MIN 16
#define MAX 32

void calc(uint mathType, float a, float b)
{
    std::cout << std::fixed << std::setprecision(2);

    if(mathType & ADD)
    {
        std::cout << "Add result: " << a + b << "\n";
    }
    if(mathType & SUB)
    {
        std::cout << "Substract result: " << a - b << "\n";
    }
    if(mathType & MUL)
    {
        std::cout << "Multiply result: " << a * b << "\n";
    }
    if(mathType & DIV)
    {
        if(b == 0)
            std::cout << "Divided by zero.\n";
        else
            std::cout << "Divide result: " << a / b << "\n";
    }
    if(mathType & MIN)
    {
        std::cout << "Min result: " << (a < b ? a : b) << "\n";
    }
    if(mathType & MAX)
    {
        std::cout << "Max result: " << (a > b ? a : b) << "\n";
    }
}

int main(int argc, char** argv)
{
    calc(ADD|SUB|MAX, 20.6, 10.751);
    return 0;
}