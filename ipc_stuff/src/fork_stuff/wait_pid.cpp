#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "t_color.h"
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{

    int pid;
    int pids[3];
    int status;
    int waitid_stt;
    int numprocess{0};
    int total_processes{3};

    while (numprocess < total_processes)
    {
        pid = fork();

        //child proc
        if(pid == 0)
        {
            sleep(2);
            std::cout << tcolor::fgreen << "Child process: " 
                                            << getpid() << tcolor::rcolor << "\n";
            sleep(5);
            return 4;
        }
        else
        {
            pids[numprocess] = pid;
            numprocess++;
            std::cout << tcolor::fmagenta << "Parent process created: " 
                                            << pid << tcolor::rcolor << "\n";
        }
    }

    waitpid(pids[total_processes - 1], &status, 0);
    if(WIFEXITED(status) != 0)
    {
        std::cout << tcolor::fyellow << "Process " << pids[total_processes - 1] 
                                    << " exited normally with status " 
                                    << WEXITSTATUS(status) << "\n";
    }
    else
    {
        std::cout << tcolor::fred << "Process " << pids[total_processes - 1] << " not exited normally.\n";
    }


    siginfo_t siginfo;
    waitid_stt = waitid(P_PID, pids[total_processes - 2], &siginfo, WEXITED);
    if(waitid_stt == -1)
    {
        perror("waitid error.\n");
        return 1;
    }

    std::cout << "Info received from waitid is: "
                << "PID of child: " << siginfo.si_pid
                << ". Real user id of child: " << siginfo.si_uid
                << "\n";

    return 0;
}