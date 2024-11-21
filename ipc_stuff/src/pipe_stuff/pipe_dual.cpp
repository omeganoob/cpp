#include <iostream>
#include <unistd.h>
#include "t_color.h"
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char** argv)
{

    int pipefds1[2];
    int pipefds2[2];

    int return_stt1, return_stt2;

    int pid;

    char msg1[20] = "message one";
    char msg2[20] = "message two";

    char readmessage[20];

    return_stt1 = pipe(pipefds1);

    if(return_stt1 == -1)
    {
        std::cout << "Can not create pipe 1.\n";
        return 1;
    }

    return_stt2 = pipe(pipefds2);

    if(return_stt2 == -1)
    {
        std::cout << "Can not create pipe 2.\n";
        return 1;
    }

    pid = fork();

    if(pid == 0)
    {
        //read from pipe 1, write to pipe 2
        close(pipefds1[1]); // close write on pipe 1
        close(pipefds2[0]); // close read on pipe 2
        read(pipefds1[0], readmessage, sizeof(readmessage));
        std::cout << "Child proc read from pipe 1 - msg: " << readmessage << "\n";

        std::cout << "Child proc wrtie to pipe 2 - msg: " << msg1 << "\n"; 
        write(pipefds2[1], msg1, sizeof(msg1));
    }
    else
    {
        //read from pipe 2, write to pipe 1
        close(pipefds1[0]); // close read on pipe 1
        close(pipefds2[1]); // close write on pipe 2
        std::cout << "Parent proc wrtie to pipe 1 - msg: " << msg2 << "\n"; 
        write(pipefds1[1], msg2, sizeof(msg2));
        
        read(pipefds2[0], readmessage, sizeof(readmessage));
        std::cout << "Parent proc read from pipe 2 - msg: " << readmessage << "\n";
    }

    return 0;
}