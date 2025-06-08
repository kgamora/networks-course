#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <stdexcept>
#include <iostream>

namespace
{
#define PROTOCOL AF_INET
// localhost, from man hosts(5)
#define LOCAL_ADDR "127.0.0.1"
#define PORT 8080
#define QUEUE_SIZE 1
#define MAX_MSG_SIZE 2 << 10

    union socket_addr
    {
        sockaddr addr;
        sockaddr_in addr_inet;
    };

}

int main()
{
    int sockfd = socket(PROTOCOL, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        throw std::runtime_error("#1");
    }

    socket_addr addr;
    {
        addr.addr_inet.sin_family = PROTOCOL;
        addr.addr_inet.sin_port = htons(PORT);
        if (int err = inet_pton(PROTOCOL, LOCAL_ADDR, &addr.addr_inet.sin_addr) <= 0)
        {
            throw std::runtime_error("#2");
        }
    }

    std::cout << "Binding to " << PORT << std::endl;

    // man 2 bind
    if (bind(sockfd, &addr.addr, sizeof(addr)) != 0)
    {
        throw std::runtime_error("#3 " + std::string(strerror(errno)));
    }

    std::cout << "Bound to " << PORT << std::endl;

    if (listen(sockfd, QUEUE_SIZE) != 0)
    {
        throw std::runtime_error("#4");
    }

    std::cout << "Listening on " << PORT << std::endl;

    // The  argument sockfd is a socket that has been created with socket(2), bound to a local address with bind(2), and is listening for connections after a listen(2).
    // accept(sockfd, )
    for (;;)
    {
        int peer_sockfd = -1;
        socket_addr peer_addr;
        socklen_t peer_addr_size = sizeof(peer_addr);

        if ((peer_sockfd = accept(sockfd, &peer_addr.addr, &peer_addr_size)) == -1)
        {
            throw std::runtime_error("#5");
        }

        std::string peer_addr_string(INET_ADDRSTRLEN, '\0');
        if (inet_ntop(PROTOCOL, &peer_addr.addr_inet.sin_addr, peer_addr_string.data(), INET_ADDRSTRLEN) == NULL)
        {
            throw std::runtime_error("#6");
        }
        std::cout << "Got connection from " << peer_addr_string << " on port " << ntohs(peer_addr.addr_inet.sin_port) << std::endl;

        std::string buffer(MAX_MSG_SIZE, '\0');

        if (recv(peer_sockfd, buffer.data(), MAX_MSG_SIZE, 0) == -1)
        {
            throw std::runtime_error("#7 " + std::string(strerror(errno)));
        }

        std::cout << "Got message " << buffer << std::endl;

        if (close(peer_sockfd) == -1)
        {
            throw std::runtime_error("#8");
        }
    }

    if (close(sockfd) == -1)
    {
        throw std::runtime_error("#101");
    }
}