#include <iostream>
#include <unistd.h>
#include "t_color.h"
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char** argv)
{

    int pipefds[2];
    int return_status;
    char writemessage[2][20] = { "Hi", "Hello" };
    char readmessage[20];

    return_status = pipe(pipefds);

    if (return_status == -1)
    {
        std::cout << "Unable to create pipe\n";
        return 1;
    }

    std::cout << "Writing to pipe - Message 1 is " << writemessage[0] << "\n";
    write(pipefds[1], writemessage[0], sizeof(writemessage[0]));
    read(pipefds[0], readmessage, sizeof(readmessage));
    std::cout << "Reading from pipe - Message 1 is " << readmessage << "\n";

    std::cout << "Writing to pipe - Message 2 is " << writemessage[1] << "\n";
    write(pipefds[1], writemessage[0], sizeof(writemessage[1]));
    read(pipefds[0], readmessage, sizeof(readmessage));
    std::cout << "Reading from pipe - Message 2 is " << readmessage << "\n";

    return 0;
}