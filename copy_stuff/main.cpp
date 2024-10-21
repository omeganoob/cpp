#include <iostream>

class Person
{
    int* m_age{};
public:
    //normal constructor
    Person(int* age = nullptr)
        : m_age{ age }
    {
        std::cout << "Person constructor: " << this << "\n";
    }

    //copy constructor
    Person(const Person& p)
    {
        m_age = new int;
        *m_age = *(p.m_age);
        std::cout << "Person copy constructor: " << this << "\n";
    }
    
    //move constructor
    Person(Person&& p) noexcept: m_age{std::move(p.m_age)}
    {
        std::cout << "Person move constructor: " << this << "\n";
        p.m_age = nullptr;
    }

    //copy assignment
    Person& operator=(const Person& p)
    {
        std::cout << "Person copy assignment\n";
        if (this == &p)
        {
            return *this;
        }

        delete m_age;

        if (p.m_age != nullptr)
        {
            m_age = new int;
            *m_age = *(p.m_age);
        }
        else
        {
            m_age = nullptr;
        }

        return *this;
    }

    //move assignment
    Person& operator=(Person&& p)
    {
        std::cout << "Person move assignment\n";
        if(this == &p)
        {
            return *this;
        }

        delete m_age;
        if(p.m_age != nullptr)
        {
            m_age = p.m_age;
            p.m_age = nullptr;
        }
        else
        {
            m_age = nullptr;
        }

        return *this;
    }

    ~Person() {
        std::cout << "Person destructor: " << this << "\n";
        delete m_age;
    }

    int* getAgePtr()
    {
        return m_age;
    }
};

Person generatePerson()
{
    Person p{new int(555555)};
    std::cout << "generatePerson return:\n";
    return p;
}

int main(int, char**) {
    
    // Person man1{ new int(2) };
    // Person man2{ new int(3) };
    // man2 = man1;
    // Person man3 = man2;
    // man1 = generatePerson();
    // int* a = new int(2);
    std::cout << "\n--------------------------\n";
    Person man2 = generatePerson();
    std::cout << "man2 address: " << &man2 << "\n";
    // std::cout << man1.getAgePtr() << "\n" << man2.getAgePtr() << "\n";
    return 0;
}