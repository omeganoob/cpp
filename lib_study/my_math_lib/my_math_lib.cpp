#include <iostream>
#include "my_math_lib.h"

int my_math::add(int a, int b)
{
    std::cout << "my_math_lib add: " << a << " + " << b << "\n";
    return a + b;
}

int my_math::sub(int a, int b)
{
    std::cout << "my_math_lib sub: " << a << " - " << b << "\n";
    return a - b;
}

float my_math::add(float a, float b)
{
    return a + b;
}
