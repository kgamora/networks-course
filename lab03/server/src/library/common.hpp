#include <arpa/inet.h>

namespace lab03
{
    union socket_addr
    {
        sockaddr addr;
        sockaddr_in addr_inet;
    };
}