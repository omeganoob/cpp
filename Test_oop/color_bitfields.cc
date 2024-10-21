#include <iostream>

struct Pixel {
    union {
        struct {
            unsigned int red : 8;
            unsigned int green : 8;
            unsigned int blue : 8;
            unsigned int alpha : 8;
        } components;
        unsigned int value;
    } color;
};

int main() {
    Pixel pixel;
    pixel.color.components.red = 255;
    pixel.color.components.green = 0;
    pixel.color.components.blue = 0;
    pixel.color.components.alpha = 255;

    std::cout << "Pixel value: " << pixel.color.value << std::endl;

    // Extract color components
    unsigned int red = pixel.color.value >> 24;
    unsigned int green = (pixel.color.value >> 16) & 0xFF;
    unsigned int blue = (pixel.color.value >> 8) & 0xFF;
    unsigned int alpha = pixel.color.value & 0xFF;

    std::cout << "Red: " << red << std::endl;
    std::cout << "Green: " << green << std::endl;
    std::cout << "Blue: " << blue << std::endl;
    std::cout << "Alpha: " << alpha << std::endl;

    return 0;
}