#include "ipc_server.h"

int main(int argc, char** argv)
{
    MQIPC::Server server{"my_server"};
    server.start();
    return 0;
}
