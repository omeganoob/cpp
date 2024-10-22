#include <iostream>
#include <cmath>
#include <stdexcept>
#include "my_dlib.h"


my_dlib::scalar_vec::scalar_vec(float x, float y)
    : _x{ x }
    , _y{ y }
{
}

float my_dlib::scalar_vec::magnitude() const
{
    return std::sqrt(_x * _x + _y * _y);
}

my_dlib::scalar_vec my_dlib::scalar_vec::normalized() const
{
    float n_x = _x / this->magnitude();
    float n_y = _y / this->magnitude();
    return scalar_vec(n_x, n_y);
}

void my_dlib::scalar_vec::print(std::ostream& os) const
{
    os << "[" << _x << ", " << _y << "]";
}

float my_dlib::scalar_vec::operator[](const char& c) const
{
    if (c == 'x')
        return _x;
    if (c == 'y')
        return _y;
    throw std::runtime_error("invalid scalar_vec member: " + c);
}

my_dlib::scalar_vec& my_dlib::scalar_vec::operator+(const scalar_vec& other)
{
    _x += other._x;
    _y += other._y;
    return *this;
}

my_dlib::scalar_vec& my_dlib::scalar_vec::operator-(const scalar_vec& other)
{
    _x -= other._x;
    _y -= other._y;
    return *this;
}

my_dlib::scalar_vec& my_dlib::scalar_vec::operator*(const scalar_vec& other)
{
    _x *= other._x;
    _y *= other._y;
    return *this;
}

my_dlib::scalar_vec& my_dlib::scalar_vec::operator/(const scalar_vec& other)
{
    _x /= other._x;
    _y /= other._y;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const my_dlib::scalar_vec& vec)
{
    vec.print(os);
    return os;
}