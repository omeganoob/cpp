
#include <string>
#include <iostream>

class Calc
{
private:
    int m_value;
public:
    static int sta;
    friend class FriendCalc;
    Calc() { m_value = 0; }
    Calc(int v) : m_value{v} {}
    Calc& add(int value) { m_value += value; return *this; }
    Calc& sub(int value) { m_value -= value; return *this; }

    friend Calc& operator+(Calc& calc1, Calc& calc2);
    friend Calc& operator+(Calc& calc, int value);

    int getVal() const 
    {
        return m_value;
    }

    void setVal(int val)
    {
        m_value = val;
    }
    void setSta(int v)
    {
        sta = v;
    }
    static int getSta()
    {
        return sta;
    }

    void printThis()
    {
        std::cout << this << "\n";
    }

    void reset()
    {
        *this = {};
    }
};

class FriendCalc
{
    private:
        int x = 2;
    public:
        int Mul(const Calc& calc)
        {
            return  calc.m_value * x;
        }
};

