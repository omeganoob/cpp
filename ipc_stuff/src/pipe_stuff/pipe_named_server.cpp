#include <iostream>
#include <unistd.h>
#include "t_color.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <fcntl.h>

#define FIFO_FILE "/my_fifo_file"

int main(int argc, char** argv)
{

    int fd;
    char readbuf[80];
    char end[5];

    int to_end;
    int read_bytes;

    mkfifo(FIFO_FILE, S_IFIFO | 0640);

    strcpy(end, "end");

    while (true)
    {
        sleep(1);
        fd = open("MYFIFO", O_RDONLY);
        read_bytes = read(fd, readbuf, sizeof(readbuf));


        if (strlen(readbuf) > 0)
        {
            readbuf[read_bytes] = '\0';
            std::cout << "Received string(" << (int)strlen(readbuf) << "): " << readbuf << "\n";

            to_end = strcmp(readbuf, end);
            if (to_end == 0)
            {
                close(fd);
                break;
            }
        }
        else
        {
            std::cerr << "Error reading from FIFO\n";
            break;
        }
    }
    return 0;
}