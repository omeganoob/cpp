#include <iostream>
#include <t_color.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <cstring>
#include <string_view>

constexpr uint MAXLENGTH = 256;

int main(int argc, char** argv)
{

    addrinfo* p, * listp, hints;

    std::vector<char> buf(MAXLENGTH);
    int rc, flags;

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <domain name>\n";
    }

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (rc = getaddrinfo(argv[1], NULL, &hints, &listp) != 0)
    {
        std::cout << "getaddrinfo error: " << gai_strerror(rc) << "\n"; 
        return 1;
    }

    flags = NI_NUMERICHOST;
    for (p = listp; p; p = p->ai_next)
    {
        buf.assign(MAXLENGTH, '\0');
        getnameinfo(p->ai_addr, p->ai_addrlen, buf.data(), MAXLENGTH, NULL, 0, flags);
        std::string_view strv(buf.data(), MAXLENGTH);
        std::cout << strv << "\n";
    }

    freeaddrinfo(listp);
    freeaddrinfo(p);

    return 0;
}