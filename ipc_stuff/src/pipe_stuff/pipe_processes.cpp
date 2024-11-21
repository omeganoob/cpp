#include <iostream>
#include <unistd.h>
#include "t_color.h"
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char** argv)
{

    int pipedfs[2];
    int return_status;
    int pid;
    char writemessage[2][20] = { "Hi", "Hello" };
    char readmessage[20];
    return_status = pipe(pipedfs);

    if (return_status == -1)
    {
        std::cout << "Unable to create pipe\n";
        return 1;
    }

    std::cout << pipedfs[0] << "\n";
    std::cout << pipedfs[1] << "\n";

    pid = fork();

    //child proc
    if (pid == 0)
    {
        std::cout << "Child proc writing to pip - MSG is " << writemessage[0] << "\n";
        write(pipedfs[1], writemessage[0], sizeof(writemessage[0]));

        read(pipedfs[0], readmessage, sizeof(readmessage));
        std::cout << "Child proc reading from pipe - MSG is " << readmessage << "\n";
    }
    else //parent proc
    {

        read(pipedfs[0], readmessage, sizeof(readmessage));
        std::cout << "Parent proc reading from pipe - MSG is " << readmessage << "\n";

        std::cout << "Parent proc writing to pip - MSG is " << writemessage[1] << "\n";
        write(pipedfs[1], writemessage[1], sizeof(writemessage[1]));
    }

    return 0;
}