#pragma once

#include "socket.hpp"

#include <memory>

namespace lab03
{
    struct ISession
    {
        virtual ~ISession() {};
        virtual void Serve() = 0;
    };

    using IUserSessionPtr = std::unique_ptr<ISession>;

    IUserSessionPtr CreateSession(TInetSocket sock, TInetAddress addr);
}