#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <fcntl.h>
#include "t_color.h"

#define FIFO_FILE "/my_fifo_file"

int main(int argc, char** argv)
{

    int fd;
    int end_process;
    int stringlen;
    char readbuf[80];
    char end_str[5];
    std::cout << "FIFO_CLIENT: Send messages, infinitely, to end enter \"end\"\n";

    fd = open(FIFO_FILE, O_CREAT | O_WRONLY);

    strcpy(end_str, "end");

    while (true)
    {
        std::cout << "Enter string: ";
        std::cin >> readbuf;
        stringlen = strlen(readbuf);
        readbuf[stringlen] = '\0';
        end_process = strcmp(readbuf, end_str);
        if (end_process != 0)
        {
            write(fd, readbuf, strlen(readbuf));
            std::cout << "Sent string(" << (int)strlen(readbuf) << "): " << readbuf << "\n";
        }
        else
        {
            write(fd, readbuf, strlen(readbuf));
            std::cout << "Sent string(" << (int)strlen(readbuf) << "): " << readbuf << "\n";
            close(fd);
            break;
        }
    }
    return 0;
}