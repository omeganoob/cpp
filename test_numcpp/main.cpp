#include <iostream>
#include <NumCpp.hpp>

int main(int, char**){

    nc::NdArray<int> a = nc::random::randInt<int>({3, 3}, 0, 10);


    std::cout << a << "\n";
    std::cout << a.shape() << "\n";

    a.reshape(1, 9);

    std::cout << a.shape() << "\n";

    auto b = nc::arange<double>(12);
    b.reshape(4, 3);

    auto c = nc::arange<double>(12);
    c.reshape(4, 3);

    std::cout << "---------------" << "\n";

    std::cout << b << "\n";
    std::cout << c << "\n";

    std::cout << nc::add(b, c) << "\n";

    auto d = nc::arange<double>(6);
    d.reshape(3, 2);

    std::cout << d << "\n";

    std::cout << nc::dot(b, d) << "\n";

    return 0;
}
