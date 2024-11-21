#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "t_color.h"
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    std::cout << "main.\n";
    std::cout << "pid: " << getpid() << "\n";
    std::cout << "ppid: " << getppid() << "\n";

    int status;
    int pid = fork();

    if(pid == 0)
    {
        std::cout << "\nchild process: " << getpid() << ". Parent: " << getppid() << "\n";
        sleep(5);
        std::cout << "\nchild process: " << getpid() << ". Parent: " << getppid() << "\n";
        return 2;
    }
    else
    {
        sleep(2);
    }

    return 0;
}