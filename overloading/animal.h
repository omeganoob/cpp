#ifndef __INHERIT_H__
#define __INHERIT_H__
// Base class representing an animal
#include <iostream>
class Animal {
public:
    int legs; // Number of legs

    Animal() {
        s_animal_cnt++;
        std::cout << "Animal constructor" << std::endl;
        legs = 0;
    }

    virtual ~Animal() {
        std::cout << "Animal destructor" << std::endl;
    }

    virtual void sound() {
        std::cout << "Animal sound" << std::endl;
    }
    static inline int s_animal_cnt{0};
};

// Derived class representing a bird
class Bird : virtual public Animal {
public:
    Bird() {
        std::cout << "Bird constructor" << std::endl;
        legs = 2;
    }

    ~Bird() {
        std::cout << "Bird destructor" << std::endl;
    }

    void sound() override {
        std::cout << "Tweet" << std::endl;
    }
};

// Derived class representing a mammal
class Mammal : virtual public Animal {
public:
    Mammal() {
        std::cout << "Mammal constructor" << std::endl;
        legs = 4;
    }

    ~Mammal() {
        std::cout << "Mammal destructor" << std::endl;
    }

    void sound() override {
        std::cout << "Growl" << std::endl;
    }
};

// Derived class representing a bat inheriting from Bird and Mammal
class Bat : public Bird, public Mammal {
public:
    Bat() {
        std::cout << "Bat constructor" << std::endl;
    }

    ~Bat() {
        std::cout << "Bat destructor" << std::endl;
    }

    void sound() override {
        // Since both Bird and Mammal inherit from Animal, we can access Animal's sound method directly
        // Animal::sound();
    }
};
#endif  