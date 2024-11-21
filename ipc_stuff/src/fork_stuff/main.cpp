#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "t_color.h"

using string = const char*;

int main(int argc, char** argv)
{
    pid_t pid, mypid, myppid;

    pid = getpid();
    std::cout << "Before fork: Process id is: " << pid << "\n";

    pid = fork();

    if(pid < 0)
    {
        perror("fork() failure.\n");
        return 1;
    }

    //child proc
    if(pid == 0)
    {
        std::cout << "This is child process\n";
        mypid = getpid();
        myppid = getppid();
        std::cout << "Process id: " << mypid << ", PPID: " << myppid << "\n";
        atexit([](){
            std::cout << tcolor::fyellow << "[Child] Called cleanup function.\n" << tcolor::rcolor;
        });
    }
    else //parent proc
    {
        sleep(1);
        std::cout << "This is parent process\n";
        mypid = getpid();
        myppid = getppid();
        std::cout << "Process id: " << mypid << ", PPID: " << myppid << "\n";
        std::cout << "Newly created process id or child pid is: " << pid << "\n";
        atexit([](){
            std::cout << tcolor::fyellow << "[Parent] Called cleanup function.\n" << tcolor::rcolor;
        });
    }

    return 0;
}