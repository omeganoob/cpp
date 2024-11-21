#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <vector>
#include <string>
#include <string_view>
#include "t_color.h"

void error(const char* msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char* argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    std::vector<char> buffer(256);
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");
    /*
        struct  hostent
        {
            char*           h_name;             official name of host
            char**          h_aliases;          alias list
            int             h_addrtype;         host address type
            int             h_length;           length of address
            char**          h_addr_list;        list of addresses from name server
            #define h_addr  h_addr_list[0]      address, for backward compatiblity
        };
    */

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char*)server->h_addr,
        (char*)&serv_addr.sin_addr.s_addr,
        server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    while (1)
    {
        std::cout << tcolor::fmagenta << "Please enter the message: " << tcolor::rcolor;

        std::string msg;

        std::getline(std::cin, msg);

        n = write(sockfd, msg.c_str(), msg.length());

        if (n < 0)
            error("ERROR writing to socket");

        if(msg == "q")
            break;

        msg.clear();

        buffer.assign(buffer.size(), '\0');

        n = read(sockfd, buffer.data(), buffer.size());

        if (n < 0)
            error("ERROR reading from socket");

        std::string_view str_v(buffer.data(), n);
        std::cout << tcolor::fgreen << str_v << tcolor::rcolor << "\n";
    }

    close(sockfd);
    return 0;
}
