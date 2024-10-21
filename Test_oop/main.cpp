#include <iostream>
#include "calc.h"

std::ostream& operator<<(std::ostream& out, Calc& calc)
{
    out << "m_value: " << calc.getVal() << "\n";
    return out;
}

int main(int argc, char** argv)
{   
    Calc calc1{2};
    Calc* p = &calc1;
    
    std::cout << p << "\n";
    calc1.printThis();
    p->add(5).add(6);
    std::cout << calc1 << "\n";
    
    calc1.reset();

    std::cout << calc1 << "\n";
    return 0;
}