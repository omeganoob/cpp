#include <iostream>
#include <memory>
#include <list>
#include <utility>
#include <vector>
#include <stack>

struct Node;

using node_ptr = std::shared_ptr<Node>;

struct Node
{
    int m_data;
    node_ptr next;
};

template<typename T>
void print_list(std::list<T>& lst)
{
    // std::list<T>::iterator iter;
    typename std::list<T>::iterator iter = lst.begin();

    for(; iter != lst.end(); iter++)
    {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

class CombinedContainer
{
    std::vector<int*> indexer{};
    std::list<int> storage{};
};

int main(int, char**)
{
    std::list<int> ilst{1, 2, 3, 4, 5, 6};
    print_list(ilst);
    return 0;
}
