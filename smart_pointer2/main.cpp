/*
#include <iostream>
#include <memory>

using namespace std;

class ClassA;
class ClassB;

class ClassA
{
public:
    std::shared_ptr<ClassB> m_data;

    ClassA() { cout << "A constructor" << endl; }
    ~ClassA() { cout << "A destructor" << endl; }
};

class ClassB
{
public:
    std::shared_ptr<ClassA> m_data;
    int m_number = 123;
    ClassB() { cout << "B constructor" << endl; }
    ~ClassB() { cout << "B destructor" << endl; }
};


int main(int, char**)
{
    shared_ptr<ClassA> sharedA(new ClassA());
    shared_ptr<ClassB> sharedB(new ClassB());

    cout << "ref count A: " << sharedA.use_count() << endl;
    cout << "ref count B: " << sharedB.use_count() << endl;

    sharedA->m_data = sharedB;
    sharedB->m_data = sharedA;

    cout << "ref count A: " << sharedA.use_count() << endl;
    cout << "ref count B: " << sharedB.use_count() << endl;

    sharedB.reset();

    auto shrPtr = sharedA->m_data.lock();
    
    if(shrPtr)
    {
        cout << "ref count B: " << sharedB.use_count() << endl;
        cout << "B number: " << shrPtr->m_number << "\n";
    }
    else
    {
        cout << "Can't get share_ptr.\n";
        cout << sharedA->m_data.expired() << "\n";
    }

    return 0;
}
*/

/*
    Example using unique_ptr t demonstrate that smartpointer is just a class
*/

#include <iostream>
#include <cstdio>
#include <memory>
#include <vector>

/*
    Custom deleter
*/
void myDeleter(FILE* file)
{
    fclose(file);
    std::cout << "myDeleter called\n";
}

/*
    Declare an alternate name for the unique_ptr
    Both are okay.
*/

// using FileManager = std::unique_ptr<FILE, void(*)(FILE*)>;
using FileManager = std::unique_ptr<FILE, decltype(myDeleter)*>;

int main(int, char**)
{
    /*
        use fopen to open file with read/write access.
        must call fclose to close the file.
    */
    FILE* file = fopen("../../../somefile.txt", "w+");

    if(!file)
    {
        std::cout << "Program failed, Error code:" << errno << "\n";
        return errno;
    }
    /*
        Create a file_manager object (a unique_ptr)
        @template params
         - object to manage
         - a deleter callable object that take a pointer to the object as parameter.
    */

    // std::unique_ptr<FILE, decltype(myDeleter)*> file_manager(file, myDeleter);
    // std::shared_ptr<FILE> file_manager(file, myDeleter);
    FileManager file_manager(file, myDeleter);
    fprintf(file_manager.get(), "%s", "nguyen quang chung");
    return 0;
}