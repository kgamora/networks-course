#include "library/session.hpp"
#include "library/socket.hpp"

#include <arpa/inet.h>

#include <iostream>
#include <thread>

namespace
{
    // localhost, from man hosts(5)
    constexpr char *LOCAL_ADDR = "127.0.0.1";
    constexpr int PORT = 8080;
    constexpr int QUEUE_SIZE = 1;
}

int main()
{
    auto sock = lab03::CreateInetSocket(SOCK_STREAM, 0);

    sock.BindTo(lab03::TInetAddress{
        .Port = PORT,
        .Address = LOCAL_ADDR,
    });

    sock.Listen(QUEUE_SIZE);

    for (;;)
    {
        auto [peer_sock, peer_addr] = sock.AcceptConnection();

        std::cout << "Got connection from " << peer_addr.Address << " on port " << peer_addr.Port << std::endl;

        std::thread sessionThread(
            [session = lab03::CreateSession(
                 std::move(peer_sock),
                 std::move(peer_addr))]
            {
                session->Serve();
            });
        sessionThread.detach();
    }
}