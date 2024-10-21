#pragma once

class Base
{
private:
    int m_id{};

public:

    Base(int id = 0)
        : m_id{ id }
    {
    }

    int getId() const { return m_id; }
};

class Derived : public Base
{
private:
    double m_cost{};
public:

    Derived(double cost = 0.0, int id = 0)
        : Base(id)
        , m_cost{ cost }
    {
    }

    double getCost() const { return m_cost; }
};