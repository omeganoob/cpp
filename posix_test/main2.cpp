#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

int main(int, char**)
{

    pid_t t = fork();

    if (t == 0)
    {
        std::cout << "child process.\n";

        if(int a = execl("../posixtest2", "posixtest2", nullptr))
        {
            std::cout << "no file: " << a << "\n";
        }
    }
    else if (t > 0)
    {
        std::cout << "main process.\n";
        int status{};
        waitpid(t, &status, 0);
        std::cout << "child done running\n";
    }
    return 0;
}