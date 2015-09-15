#include "httpserver.h"

#include <algorithm>
#include <assert.h>
#include <list>
#include <vector>

#include "socketx.h"

const int POST_MAX_LENGTH = 10000000;

namespace {

std::vector<std::string> split(const std::string &s, char d)
{
    std::vector<std::string> r;
    std::string::size_type i = 0;
    auto start = i;
    while (i < s.length()) {
        if (s.at(i) == d) {
            r.push_back(s.substr(start, i-start));
            start = i + 1;
        }
        i++;
    }
    if (i > start) {
        r.push_back(s.substr(start, i-start));
    }
    return r;
}

} // namespace

std::string HttpResponse::to_string() const
{
    std::string r;
    r.append("HTTP/1.0 " + std::to_string(code) + " ok\r\n");
    r.append("Content-length: " + std::to_string(content.length()) + "\r\n");
    r.append("\r\n");
    r.append(content);
    return r;
}

class Client {
public:
    Client(IHttpServerHandler *handler, SOCKET socket): handler(handler), socket(socket), request(), path(), post_length(-1), post_data() {}
    ~Client();
    IHttpServerHandler *handler;
    SOCKET socket;
    std::string request;
    std::string path;
    int post_length;
    std::string post_data;

    bool handle_data(const std::string &data);
    bool handle_request();
    void handle_response(HttpResponse &response);
    static bool header_match(const std::string &header, const std::string &target, std::string &value);
private:
    Client(const Client &);
    Client &operator=(const Client &);
};

Client::~Client()
{
    closesocket(socket);
}

bool Client::handle_data(const std::string &data)
{
    if (post_length < 0) {
        request.append(data);
        auto end = request.find("\r\n\r\n");
        if (end != std::string::npos) {
            end += 4;
            post_data = request.substr(end);
            request = request.substr(0, end);
            if (handle_request()) {
                return true;
            }
        }
    } else {
        post_data.append(data);
        post_length -= static_cast<int>(data.length());
        if (post_length <= 0) {
            HttpResponse response;
            handler->handle_POST(path, post_data, response);
            handle_response(response);
            return true;
        }
    }
    return false;
}

bool Client::handle_request()
{
    std::vector<std::string> lines;
    std::string::size_type i = 0;
    for (;;) {
        auto start = i;
        i = request.find("\r\n", i);
        if (i == std::string::npos || i == start) {
            break;
        }
        lines.push_back(request.substr(start, i));
        i += 2;
    }
    std::vector<std::string> request = split(lines[0], ' ');
    const std::string &method = request[0];
    path = request[1];
    //const std::string &version = request[2];
    HttpResponse response;
    if (method == "GET") {
        handler->handle_GET(path, response);
    } else if (method == "POST") {
        for (auto &h: lines) {
            std::string value;
            if (header_match(h, "Content-Length:", value)) {
                int length = std::stoi(value);
                if (length >= 0 && length <= POST_MAX_LENGTH) {
                    post_length = length;
                    post_length -= post_data.length();
                    if (post_length > 0) {
                        return false;
                    } else {
                        handler->handle_POST(path, post_data, response);
                    }
                }
                break;
            }
        }
    } else {
        response.code = 404;
    }
    handle_response(response);
    return true;
}

void Client::handle_response(HttpResponse &response)
{
    std::string r = response.to_string();
    send(socket, r.c_str(), static_cast<int>(r.length()), 0);
}

bool Client::header_match(const std::string &header, const std::string &target, std::string &value)
{
    std::string h = header.substr(0, target.length());
    std::string t = target;
    std::transform(h.begin(), h.end(), h.begin(), tolower);
    std::transform(t.begin(), t.end(), t.begin(), tolower);
    if (h == t) {
        auto i = target.length();
        while (i < header.length() && isspace(header[i])) {
            ++i;
        }
        value = header.substr(i);
        return true;
    } else {
        return false;
    }
}

class HttpServerImpl {
public:
    HttpServerImpl(unsigned short port, IHttpServerHandler *handler);
    ~HttpServerImpl();

    IHttpServerHandler *const handler;
    SOCKET server;
    std::list<Client> clients;

    void service(bool wait);
    void handle_request(Client &client);

private:
    HttpServerImpl(const HttpServerImpl &);
    HttpServerImpl &operator=(const HttpServerImpl &);
};

HttpServerImpl::HttpServerImpl(unsigned short port, IHttpServerHandler *handler)
  : handler(handler),
    server(),
    clients()
{
#ifdef _WIN32
    static bool initialized = false;
    if (!initialized) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(1, 1), &wsa);
        initialized = true;
    }
#endif
    server = socket(PF_INET, SOCK_STREAM, 0);
    assert(server != INVALID_SOCKET);
    int on = 1;
    int r = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&on), sizeof(on));
    assert(r == 0);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    r = bind(server, reinterpret_cast<const sockaddr *>(&sin), sizeof(sin));
    if (r != 0) {
        perror("bind");
        closesocket(server);
        server = INVALID_SOCKET;
        return;
    }
    r = listen(server, 5);
    assert(r == 0);
}

HttpServerImpl::~HttpServerImpl()
{
    if (server != INVALID_SOCKET) {
        closesocket(server);
    }
}

void HttpServerImpl::service(bool wait)
{
    if (server == INVALID_SOCKET) {
        return;
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(server, &rfds);
    SOCKET maxsocket = server;
    for (auto &c: clients) {
        FD_SET(c.socket, &rfds);
        if (c.socket > maxsocket) {
            maxsocket = c.socket;
        }
    }
    timeval tv = {0, 0};
    int n = select(static_cast<int>(maxsocket+1), &rfds, NULL, NULL, wait ? NULL : &tv);
    if (n > 0) {
        if (FD_ISSET(server, &rfds)) {
            sockaddr_in sin;
            socklen_t socklen = sizeof(sin);
            SOCKET t = accept(server, reinterpret_cast<sockaddr *>(&sin), &socklen);
            assert(t != INVALID_SOCKET);
            clients.emplace_back(handler, t);
        }
        for (auto i = clients.begin(); i != clients.end(); ) {
            if (FD_ISSET(i->socket, &rfds)) {
                char buf[200];
                int n = recv(i->socket, buf, sizeof(buf), 0);
                if (n > 0) {
                    if (i->handle_data(std::string(buf, n))) {
                        auto d = i;
                        ++i;
                        clients.erase(d);
                    } else {
                        ++i;
                    }
                } else {
                    auto d = i;
                    ++i;
                    clients.erase(d);
                }
            } else {
                ++i;
            }
        }
    }
}

HttpServer::HttpServer(unsigned short port, IHttpServerHandler *handler)
  : impl(new HttpServerImpl(port, handler))
{
}

HttpServer::~HttpServer()
{
    delete impl;
}

void HttpServer::service(bool wait)
{
    impl->service(wait);
}
