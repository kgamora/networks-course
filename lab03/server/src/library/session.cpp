#include "session.hpp"

#include "common.hpp"

#include <httpparser/request.h>
#include <httpparser/response.h>
#include <httpparser/httprequestparser.h>

#include <optional>
#include <filesystem>
#include <fstream>

namespace
{
    // 4 KiB. Arbitrary value that I like
    constexpr size_t MAX_MSG_SIZE = (1 << 12);

    [[nodiscard]] inline std::string ToHex(size_t value)
    {
        std::ostringstream res;
        res << std::hex << value;
        return res.str();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    httpparser::Response ResponseDefault()
    {
        httpparser::Response response;
        response.versionMajor = 1;
        response.versionMinor = 1;
        return response;
    };

    httpparser::Response MakeNotFoundError()
    {
        auto result = ResponseDefault();
        result.statusCode = 404;
        result.status = "Not Found";
        return result;
    }

    httpparser::Response MakeNotImplementedError()
    {
        auto result = ResponseDefault();
        result.statusCode = 501;
        result.status = "Not Implemented";
        return result;
    }

    httpparser::Response MakeBadRequestError()
    {
        auto result = ResponseDefault();
        result.statusCode = 400;
        result.status = "Bad Request";
        return result;
    }

    httpparser::Response MakeOK()
    {
        auto result = ResponseDefault();
        result.statusCode = 200;
        result.status = "OK";
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string ReadFile(const std::filesystem::path &path, size_t chunkIndex = 0, size_t maxSize = MAX_MSG_SIZE)
    {
        auto file = std::ifstream(path);
        std::string result(maxSize, '\0');
        file.seekg(chunkIndex * maxSize);
        file.read(result.data(), maxSize);
        auto actualSize = file.gcount();
        result.resize(actualSize);
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    using TErrorResponseOr = lab03::TEither<httpparser::Response, T>;

    TErrorResponseOr<std::filesystem::path> CheckPath(httpparser::Request request)
    {
        auto path = std::filesystem::path(request.uri);

        if (!std::filesystem::exists(path))
        {
            return MakeNotFoundError();
        }

        if (!std::filesystem::is_regular_file(path))
        {
            return MakeNotImplementedError();
        }

        return std::move(path);
    }

    TErrorResponseOr<httpparser::Request> ParseRequest(std::string request_bytes)
    {
        httpparser::Request request;
        httpparser::HttpRequestParser parser;

        httpparser::HttpRequestParser::ParseResult res = parser.parse(request, request_bytes.data(), request_bytes.data() + request_bytes.size());

        if (res != httpparser::HttpRequestParser::ParsingCompleted)
        {
            return MakeBadRequestError();
        }

        return std::move(request);
    }

    TErrorResponseOr<httpparser::Request> CheckRequest(httpparser::Request request)
    {
        if (request.method != "GET")
        {
            return MakeNotImplementedError();
        }
        return std::move(request);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class TSession final : public lab03::ISession
    {
        const lab03::TInetSocket PeerSocket;
        const lab03::TInetAddress PeerAddress;

    private:
        void SendRaw(std::string message)
        {
            if (send(PeerSocket, message.data(), message.size(), 0) != -1)
            {
                return;
            }

            throw std::runtime_error("Failed to send message to peer: " + std::string(strerror(errno)));
        }

        void Send(httpparser::Response response)
        {
            SendRaw(response.inspect());
        }

        TErrorResponseOr<std::string> Receive()
        {
            std::string messageBytes(MAX_MSG_SIZE, '\0');
            int64_t messageSize;
            if ((messageSize = recv(PeerSocket, messageBytes.data(), MAX_MSG_SIZE, 0)) == -1)
            {
                throw std::runtime_error("Failed to receive message from peer: " + std::string(strerror(errno)));
            }

            messageBytes.resize(messageSize);

            return messageBytes;
        }

    public:
        TSession(lab03::TInetSocket sock, lab03::TInetAddress addr) : PeerSocket(std::move(sock)),
                                                                      PeerAddress(std::move(addr)) {};

        void Serve() override
        {
            using namespace httpparser;

            auto maybePath = Receive()
                                 .AndThen(ParseRequest)
                                 .AndThen(CheckRequest)
                                 .AndThen(CheckPath);

            if (maybePath.HasLeft())
            {
                Send(maybePath.ExtractLeft());
                return;
            }

            auto path = maybePath.ExtractRight();

            // If something went wrong when reading the file, crash

            auto fileSize = std::filesystem::file_size(path);

            auto response = MakeOK();
            response.headers.emplace_back(Response::HeaderItem{"Content-Type", "text/plain"});
            response.headers.emplace_back(Response::HeaderItem{"Transfer-Encoding", "chunked"});

            // Send HTTP response
            Send(response);

            // Integer division, round up
            size_t nChunks = fileSize / MAX_MSG_SIZE + (fileSize % MAX_MSG_SIZE != 0);

            for (size_t i = 0; i < nChunks; i++)
            {
                // Send chunks
                std::string chunkData = ReadFile(path, i, MAX_MSG_SIZE);
                std::string chunkSize = ToHex(chunkData.size());

                SendRaw(chunkSize + "\r\n" + chunkData + "\r\n");
            }

            // Send empty chunk to indicate EOF
            SendRaw("0\r\n\r\n");
        }
    };
}

namespace lab03
{
    IUserSessionPtr CreateSession(TInetSocket sock, TInetAddress addr)
    {
        return std::make_unique<TSession>(std::move(sock), std::move(addr));
    }
}
