#pragma once

#include <cstdint>
#include <string>

namespace lab03
{
    struct TInetAddress
    {
        uint16_t Port;
        std::string Address;
    };

    class TInetSocket
    {
        int FileDescriptor;

    public:
        explicit TInetSocket(int socket) : FileDescriptor(socket) {};
        TInetSocket(TInetSocket &&other) : FileDescriptor(other.FileDescriptor)
        {
            // Dirty way of preventing destructor from closing file descriptors of temporary socket objects
            other.FileDescriptor = -1;
        }
        ~TInetSocket();

        operator int() const { return FileDescriptor; }
        void BindTo(const TInetAddress &address);
        void Listen(int queueSize);

        std::pair<TInetSocket, TInetAddress> AcceptConnection();
    };

    TInetSocket CreateInetSocket(int type, int protocol);
}