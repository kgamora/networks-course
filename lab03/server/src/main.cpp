#include "library/socket.hpp"

#include <httpparser/request.h>
#include <httpparser/response.h>
#include <httpparser/httprequestparser.h>

#include <arpa/inet.h>

#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace
{
// localhost, from man hosts(5)
#define LOCAL_ADDR "127.0.0.1"
#define PORT 8080
#define QUEUE_SIZE 1
// 4 KiB. Arbitrary value that I like
#define MAX_MSG_SIZE (4 * (2 << 10))

    std::string not_implemented(httpparser::Response response)
    {
        response.statusCode = 501;
        response.status = "Not Implemented";
        return response.inspect();
    }

    std::string not_found(httpparser::Response response)
    {
        response.statusCode = 404;
        response.status = "Not Found";
        return response.inspect();
    }

    std::string read_file(const std::filesystem::path &path)
    {
        auto file = std::ifstream(path);
        std::string result(MAX_MSG_SIZE, '\0');
        file.read(result.data(), MAX_MSG_SIZE);
        result.resize(file.gcount());
        return result;
    }

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

        if (request.method != "GET")
        {
            return not_implemented(std::move(response));
        }

        auto path = std::filesystem::path(request.uri);

        if (!std::filesystem::exists(path))
        {
            return not_found(std::move(response));
        }

        if (!std::filesystem::is_regular_file(path))
        {
            return not_implemented(std::move(response));
        }

        // If something went wrong when reading the file, crash

        auto fileSize = std::filesystem::file_size(path);

        response.statusCode = 200;
        response.status = "OK";

        // if (fileSize <= MAX_MSG_SIZE)
        // {
        // }

        std::string messageBody = read_file(path);
        // Library has a bug =)
        auto content = "\r\n" + messageBody;
        response.content = {content.begin(), content.end()};
        response.headers.emplace_back(Response::HeaderItem{"Content-Length", std::to_string(messageBody.size())});
        response.headers.emplace_back(Response::HeaderItem{"Content-Type", "text/plain"});

        return response.inspect();

        // // chunked transporting
        // auto nChunks = fileSize / MAX_MSG_SIZE;
        // for (size_t i = 0; i < fileSize; i++)
        // {
        // }
    }
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

        std::string msg(MAX_MSG_SIZE, '\0');

        int64_t msg_size;
        if ((msg_size = recv(peer_sock, msg.data(), MAX_MSG_SIZE, 0)) == -1)
        {
            throw std::runtime_error("Failed to receive message from peer: " + std::string(strerror(errno)));
        }

        msg.resize(msg_size);

        auto answer = handle_msg(std::move(msg));

        send(peer_sock, answer.data(), answer.size(), 0);
    }
}