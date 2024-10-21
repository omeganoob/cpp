#include "calc.h"

int Calc::sta{4};

Calc& operator+(Calc& calc1, Calc& calc2)
{
    calc1.m_value += calc2.m_value;
    return calc1;
}

Calc& operator+(Calc& calc, int value)
{
    calc.m_value += value;
    return calc;
}
