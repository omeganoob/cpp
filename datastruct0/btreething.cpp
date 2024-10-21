#include <iostream>
#include <memory>

struct Node;

using node_ptr = std::shared_ptr<Node>;
#define createNode std::make_shared<Node>
struct Node
{
    Node(int data) : m_data{data} {}

    
    int m_data{};
    node_ptr left;
    node_ptr right;
};

struct Tree
{
    node_ptr root{nullptr};
    void insert(node_ptr node)
    {
        root = handle_insert(root, node);
    }
    void print() 
    {
        handle_print(root, "", true);
        std::cout << "done.\n";
    }
    bool search(int data)
    {
        return handle_search(root, data);
    }
    void remove(int data)
    {
        if(!search(data))
        {
            std::cout << "Can't remove: No node with data of " << data << " found.\n";
            return;
        }
        handle_remove(root, data); 
    }
private:
    node_ptr handle_insert(node_ptr root, node_ptr node)
    {
        int data = node->m_data;

        if(root == nullptr)
        {
            root = node;
            return root;
        }
        else if (data < root->m_data)
        {
            root->left = handle_insert(root->left, node);
        }
        else
        {
            root->right = handle_insert(root->right, node);
        }

        return root;
    }
    void handle_print(node_ptr root, std::string prefix, bool isLeft)
    {
        if(!root)
        {
            return;
        }
 
        std::cout << prefix;
        std::cout << (isLeft ? "└── " : "┌── " );
        std::cout << root->m_data << std::endl;

        handle_print(root->right, prefix + (isLeft ? "    " : "│   "), false);
        handle_print(root->left, prefix + (isLeft ? "    " : "│   "), true);
    }
    bool handle_search(node_ptr root, const int& data)
    {
        if(!root) 
            return false;

        if(root->m_data == data)
            return true;
        
        if(data < root->m_data)
        {
            return handle_search(root->left, data);
        }
        else
        {
            return handle_search(root->right, data);
        }
    }
    node_ptr handle_remove(node_ptr root, const int& data)
    {
        if(!root)
            return root;

        if(data == root->m_data)
        {
            if(root->left == nullptr && root->right == nullptr)
            {
                root = nullptr;
                return root;
            }
            else if(root->right != nullptr)
            {
                root->m_data = successor(root);
                root->right = handle_remove(root->right, root->m_data);
            }
            else
            {
                root->m_data = predecessor(root);
                root->left = handle_remove(root->left, root->m_data);
            }
            return root;
        }

        if(data < root->m_data)
        {
            root->left = handle_remove(root->left, data);
            return root;
        }
        else if (data > root->m_data)
        {
            root->right = handle_remove(root->right, data);
            return root;
        }

        return nullptr;
    }
    int successor(node_ptr root)
    {
        root = root->right;
        while(root->left != nullptr)
        {
            root = root->left;
        }
        return root->m_data;
    }
    int predecessor(node_ptr root)
    {
        root = root->left;
        while(root->right != nullptr)
        {
            root = root->right;
        }
        return root->m_data;
    }
};

int main(int, char**)
{
    Tree tree;

    tree.insert(createNode(5));
    tree.insert(createNode(4));
    tree.insert(createNode(3));
    tree.insert(createNode(6));
    tree.insert(createNode(1));
    tree.insert(createNode(2));
    tree.insert(createNode(7));
    tree.insert(createNode(8));
    tree.insert(createNode(9));
    
    tree.print();

    // std::cout << (tree.search(2) ? "founded" : "not found") << "\n";
    
    // tree.remove(11);
    
    // tree.remove(5);

    // tree.print();

    return 0;
}