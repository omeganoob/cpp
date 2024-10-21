#include <iostream>
#include <vector>
#include <forward_list>
#include <array>
#include <memory>

template<typename T>
concept CanPrint = requires (T t)
{
    std::cout << t;
};

class Book
{
public:
    std::string name{};
    std::string author{};
    void print(std::ostream& os) const
    {
        os << "name: " << name << ". author: " << author << ".\n";
    }
};

std::ostream& operator<<(std::ostream& os, const Book& book)
{
    book.print(os);
    return os;
}


template<CanPrint T>
void print_vec(const std::vector<T>& vec)
{
    for (auto &&i : vec)
    {
        std::cout << i << "\n";
    }
    
}

int main(int, char**)
{
    std::vector<Book> book_vec{{"sach0", "author0"}, {"sach1", "author1"}};
    print_vec(book_vec);
    std::cout << "-------------------\n";

    Book& abook = book_vec[0];

    book_vec.push_back( {"sach2", "author2"} );
    book_vec.emplace_back("sach3", "author3");

    std::cout << &abook << "\n";
    std::cout << &book_vec[0] << "\n";
    std::cout << &(*book_vec.begin()) << "\n";

    return 0;
}