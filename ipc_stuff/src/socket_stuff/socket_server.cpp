/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string_view>
#include <cstring>
#include "t_color.h"

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

void dostuff(int sock)
{
    ssize_t n;
    std::vector<char> buffer(256);
    buffer.assign(buffer.size(), '\0');

    while (1)
    {
        n = read(sock, buffer.data(), buffer.size());
        if (n < 0) error("ERROR reading from socket");

        std::string_view str_v(buffer.data(), n);

        std::cout << tcolor::fyellow << "Here is the message: " << str_v << tcolor::rcolor << "\n";
        // printf("Here is the message: %s\n", buffer.data());

        if (str_v == "q")
        {
            std::cout << tcolor::fred << "a client disconnect.\n" << tcolor::rcolor;
            break;
        }

        n = write(sock, "I got your message", 18);

        if (n < 0)
        {
            error("ERROR writing to socket");
            break;
        }
    }
    close(sock);
}

int main(int argc, char* argv[])
{
    int sockfd, newsockfd, portno;
    pid_t pid;
    socklen_t clilen;
    //  char buffer[256];
    std::vector<char> buffer(256);
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char*)&serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serv_addr,
        sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    while (true)
    {
        newsockfd = accept(sockfd,
            (struct sockaddr*)&cli_addr,
            &clilen);

        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }

        pid = fork();

        if (pid < 0)
        {
            error("Error on fork()");
        }
        if (pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd);
            return 0;
        }
        else
        {
            close(newsockfd);
        }
    }
    close(sockfd);
    return 0;
}
