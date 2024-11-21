#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

int main(int argc, char** argv)
{
    std::string path = "/proc/" + 
                      std::string(argv[1]) + 
                      "/fd/0";
    
    int fd = open(path.c_str(),
                 O_WRONLY | O_CREAT,
                 0644);
    
    if (fd == -1) {
        std::cerr << "Failed to open file: "
                  << strerror(errno)
                  << " " << path << std::endl;
        return 1;
    }
    
    std::string str;
    
    while (true) {
        std::getline(std::cin, str);

        if(str == "q")
        {
            write(fd, str.c_str(), str.length());
            break;
        }

        str += "\n";
        write(fd, str.c_str(), str.length());
        str = "";
    }
    
    close(fd);
    return 0;
}