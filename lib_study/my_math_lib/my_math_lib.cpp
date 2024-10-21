#include <iostream>
#include "my_math_lib.h"

void say_hello(){
    std::cout << "Hello, from my_math_lib!\n";
}

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
