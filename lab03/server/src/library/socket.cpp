#include "socket.hpp"
#include "common.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>
#include <cstring>

namespace
{
    constexpr int PROTOCOL = AF_INET;

    lab03::socket_addr toSocketAddr(const lab03::TInetAddress &inetAddress)
    {
        using namespace lab03;
        socket_addr result;
        {
            result.addr_inet.sin_family = PROTOCOL;
            result.addr_inet.sin_port = htons(inetAddress.Port);
            if (int err = inet_pton(PROTOCOL, inetAddress.Address.c_str(), &result.addr_inet.sin_addr) <= 0)
            {
                throw std::runtime_error("Unable to construct inet address" + std::string(std::strerror(errno)));
            }
        }
        return result;
    }

    lab03::TInetAddress toInetAddr(const lab03::socket_addr &sockAddr)
    {
        using namespace lab03;
        std::string peer_addr_string(INET_ADDRSTRLEN, '\0');

        if (inet_ntop(PROTOCOL, &sockAddr.addr_inet.sin_addr, peer_addr_string.data(), INET_ADDRSTRLEN) != NULL)
        {
            return {
                .Port = ntohs(sockAddr.addr_inet.sin_port),
                .Address = std::move(peer_addr_string),
            };
        }

        throw std::runtime_error("Unable to read connected peer inet address: " + std::string(std::strerror(errno)));
    }
}

namespace lab03
{
    TInetSocket::~TInetSocket()
    {
        if (FileDescriptor == -1 || close(FileDescriptor) != -1)
        {
            return;
        }
        throw std::runtime_error("Failed to close socket file descriptor: " + std::string(std::strerror(errno)));
    }

    void TInetSocket::BindTo(const TInetAddress &address)
    {
        auto addr = toSocketAddr(address);
        // man 2 bind
        if (bind(FileDescriptor, &addr.addr, sizeof(addr)) == 0)
        {
            return;
        }
        throw std::runtime_error("Failed to bind socket: " + std::string(std::strerror(errno)));
    }

    void TInetSocket::Listen(int queueSize)
    {
        if (listen(FileDescriptor, queueSize) == 0)
        {
            return;
        }
        throw std::runtime_error("Failed to listen on socket: " + std::string(std::strerror(errno)));
    }

    std::pair<TInetSocket, TInetAddress> TInetSocket::AcceptConnection()
    {
        socket_addr peer_addr;
        socklen_t peer_addr_size = sizeof(peer_addr);

        int peer_sock = accept(FileDescriptor, &peer_addr.addr, &peer_addr_size);
        if (peer_sock == -1)
        {
            throw std::runtime_error("Failed to accept connection: " + std::string(std::strerror(errno)));
        }

        return {TInetSocket{peer_sock}, toInetAddr(peer_addr)};
    }

    ////////////////////////////////////////////////////////////////////////////

    TInetSocket CreateInetSocket(int type, int protocol)
    {
        int sockfd = socket(PROTOCOL, type, protocol);
        if (sockfd != -1)
        {
            return TInetSocket{sockfd};
        }
        throw std::runtime_error("Failed to create socket: " + std::string(std::strerror(errno)));
    }
}