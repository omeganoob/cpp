#include "my_math_lib.h"
#include <iostream>

int my_math::add(int a, int b)
{
    std::cout << "my_math_lib: " << a << " + " << b << "\n";
    return a + b;
}

int my_math::sub(int a, int b)
{
    std::cout << "my_math_lib: " << a << " - " << b << "\n";
    return a - b;
}
