#include <concepts>
#include <vector>
#include <iostream>

template<typename T>
concept SupportsLessThan = requires (T x) { x < x; };

template<typename T>
requires std::copyable<T>&& SupportsLessThan<T>
T mymax(T a, T b)
{
    return a > b ? a : b;
}

template<class T1, class T2>
void print(const T1& val1, const T2& val2)
{
    std::cout << val1 << " " << val2 << "\n";
}

template<typename T1, typename T2>
auto mymin(T1 a, T2 b)
{
    return a < b ? a : b;
}

template<typename T>
class Stack
{
private:
    std::vector<T> elems;
public:
    Stack();
    void push(const T&);
    T pop();
    T top() const;
    bool empty() const
    {
        return elems.empty();
    }
    void print() const
    {
        for (const T& elem : elems)
        {
            std::cout << elem << " "; // requires << for type T
        }
        std::cout << std::endl;
    }
};

template<typename T, size_t SZ, int ID>
struct MyArray
{
    using iterator = T*;

    T elems[SZ];

    const size_t size() const
    {
        return SZ;
    }

    const int id() const
    {
        return ID;
    }

    T& operator[] (size_t idx)
    {
        return elems[idx];
    }

    const T& operator[] (size_t idx) const
    {
        return elems[idx];
    }

    T* begin()
    {
        return &elems[0];
    }

    T* end()
    {
        return &elems[0] + SZ;
    }
};

//Variadic template
template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
    std::cout << firstArg << "\n";
    // print(args...);
    if /*constexpr*/ (sizeof...(args) > 0)
    {
        print(args...);
    }
}

template<typename Coll>
concept HasPushback = requires (Coll c, Coll::value_type v)
{
    c.push_back(v);
};

void add(HasPushback auto& coll, const auto& val) // clarify from ambigous func call
{
    coll.push_back(val);
}

/*
Above same as:
template<HasPushback T1, typename T2>
void add(T1& coll, const T2& val)
{
    coll.push_back(val);
}
*/

void add(auto& coll, const auto& val)
{
    coll.insert(val);
}

// OR use requires with Compile-time if:

/*
void add(auto& coll, const auto& val)
{
    if constexpr (requires  {coll.push_back(val); })
    {
        coll.push_back(val);
    }
    else {
        coll.insert(val);
    }
}
*/