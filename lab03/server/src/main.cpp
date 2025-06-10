#include <httpparser/request.h>
#include <httpparser/response.h>
#include <httpparser/httprequestparser.h>

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

    std::string handle_msg(std::string http_request)
    {
        using namespace httpparser;

        Request request;
        HttpRequestParser parser;

        HttpRequestParser::ParseResult res = parser.parse(request, http_request.data(), http_request.data() + http_request.size());

        if (res != HttpRequestParser::ParsingCompleted)
        {
            throw std::runtime_error("Failed to parse HTTP");
        }

        Response response;
        response.versionMajor = 1;
        response.versionMinor = 1;
        response.statusCode = 200;
        response.status = "OK";

        // Library has a bug =)
        auto content = "\r\nGot method " + request.method + " on path: " + request.uri;
        response.content = {content.begin(), content.end()};
        response.headers.emplace_back(Response::HeaderItem{"Content-Length", std::to_string(content.size() - 2)});
        response.headers.emplace_back(Response::HeaderItem{"Content-Type", "text/plain"});

        std::cout << content << std::endl;

        return response.inspect();
    }
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

        std::string msg(MAX_MSG_SIZE, '\0');

        int64_t msg_size;
        if ((msg_size = recv(peer_sockfd, msg.data(), MAX_MSG_SIZE, 0)) == -1)
        {
            throw std::runtime_error("#7 " + std::string(strerror(errno)));
        }

        msg.resize(msg_size);

        auto answer = handle_msg(std::move(msg));

        send(peer_sockfd, answer.data(), answer.size(), 0);

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