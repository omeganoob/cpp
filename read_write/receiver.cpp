#include <iostream>
#include <string>
#include <unistd.h>

int main(int, char**)
{
    std::cout << getpid() << "\n";
     
    std::string str;
    
    while(true)
    {
        std::getline(std::cin, str);
        if(str == "q") break;
        std::cout << str << std::endl;
        str = "";
    }
    
    return 0;
}