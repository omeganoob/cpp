#include <iostream>

class Animal {
public:
    virtual void makeSound() const
    {
        std::cout << "???" << std::endl;
    };
};

class Dog : public Animal {
public:
    void makeSound() const override { std::cout << "Woof!" << std::endl; }
};

class Cat : public Animal {
public:
    void makeSound() const override { std::cout << "Meow!" << std::endl; }
};

void printAnimalSound(const Animal animal) {
    animal.makeSound(); // Object slicing occurs here
}

int main() {
    
    Dog dog;
    Cat cat;

    // Passing a copy of the dog object to the function
    printAnimalSound(dog);

    return 0;
}