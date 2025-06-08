#pragma once

#include <arpa/inet.h>

#include <variant>

namespace lab03
{
    union socket_addr
    {
        sockaddr addr;
        sockaddr_in addr_inet;
    };

    template <typename L, typename R>
    class TEither
    {
        std::variant<L, R> Value;

    public:
        TEither(L l) : Value(std::move(l)) {};
        TEither(R r) : Value(std::move(r)) {};

        template <typename RY>
        TEither<L, RY> AndThen(TEither<L, RY> (*f)(R))
        {
            if (HasLeft())
            {
                return {std::move(std::get<0>(Value))};
            }
            return f(std::get<1>(Value));
        }

        [[nodiscard]] bool HasLeft() const
        {
            return Value.index() == 0;
        }

        [[nodiscard]] bool HasRight() const
        {
            return !HasLeft();
        }

        [[nodiscard]] L ExtractLeft()
        {
            return std::move(std::get<0>(Value));
        }

        [[nodiscard]] R ExtractRight()
        {
            return std::move(std::get<1>(Value));
        }
    };
}