#include <iostream>
namespace my_share {

    template<typename T>
    class MySharedPointer
    {
        T* m_ptr;
        int* m_refCount;  // Reference count

    public:
        // Constructor
        MySharedPointer(T* ptr = nullptr)
            : m_ptr(ptr), m_refCount(new int(1))  // Initialize reference count to 1
        {
            std::cout << "pointer created. RefCount = " << *m_refCount << "\n";
        }

        // Destructor
        ~MySharedPointer()
        {
            // Decrement the reference count
            if (--(*m_refCount) == 0)
            {
                delete m_ptr;        // Delete the resource
                delete m_refCount;   // Delete the reference count
                std::cout << "pointer destroyed.\n";
            }
            else
            {
                std::cout << "pointer not destroyed. RefCount = " << *m_refCount << "\n";
            }
        }

        // Copy constructor
        MySharedPointer(const MySharedPointer& p)
            : m_ptr(p.m_ptr), m_refCount(p.m_refCount)
        {
            // Increment the reference count
            ++(*m_refCount);
            std::cout << "copy constructor called. RefCount = " << *m_refCount << "\n";
        }

        // Copy assignment operator
        MySharedPointer& operator=(const MySharedPointer& p)
        {
            if (this == &p)
                return *this;

            // Decrement old reference count and delete if necessary
            if (--(*m_refCount) == 0)
            {
                delete m_ptr;
                delete m_refCount;
            }

            // Copy the resource and reference count from p
            m_ptr = p.m_ptr;
            m_refCount = p.m_refCount;
            ++(*m_refCount);  // Increment the reference count

            std::cout << "copy assignment called. RefCount = " << *m_refCount << "\n";
            return *this;
        }

        // Move constructor
        MySharedPointer(MySharedPointer&& p) noexcept
            : m_ptr(p.m_ptr), m_refCount(p.m_refCount)
        {
            p.m_ptr = nullptr;
            p.m_refCount = nullptr;
            std::cout << "move constructor called.\n";
        }

        // Move assignment operator
        MySharedPointer& operator=(MySharedPointer&& p) noexcept
        {
            if (this == &p)
                return *this;

            // Decrement old reference count and delete if necessary
            if (--(*m_refCount) == 0)
            {
                delete m_ptr;
                delete m_refCount;
            }

            // Transfer ownership
            m_ptr = p.m_ptr;
            m_refCount = p.m_refCount;

            p.m_ptr = nullptr;
            p.m_refCount = nullptr;

            std::cout << "move assignment called.\n";
            return *this;
        }

        // Dereference operators
        T& operator*() const
        {
            return *m_ptr;
        }

        T* operator->() const
        {
            return m_ptr;
        }

        // Get the reference count (for debugging purposes)
        int getRefCount() const
        {
            return *m_refCount;
        }
    };

}