#include <stdexcept>
#include <cmath>
#include <iostream>


namespace my_dlib
{
    class scalar_vec
    {
    public:
        scalar_vec(float x, float y);
        float magnitude() const;
        scalar_vec normalized() const;
        void print(std::ostream& os) const;

        float operator[] (const char& c) const;
        scalar_vec& operator+ (const scalar_vec& other);
        scalar_vec& operator- (const scalar_vec& other);
        scalar_vec& operator* (const scalar_vec& other);
        scalar_vec& operator/ (const scalar_vec& other);
    private:
        float _x{};
        float _y{};
    };

    extern "C" const int getVecSize()
    {
        return sizeof(scalar_vec);
    }
}

std::ostream& operator<<(std::ostream&, const my_dlib::scalar_vec& vec);