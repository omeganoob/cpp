#include <iostream>
#include "my_math_lib.h"
#include "my_dlib.h"

int main(int, char**) {
    std::cout << "Hello, from test_lib!\n";
    std::cout << my_math::add(1, 3) << "\n";
    my_dlib::scalar_vec vec{3, 4};
    std::cout << vec << "\n";
}
