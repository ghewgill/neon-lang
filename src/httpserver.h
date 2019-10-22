#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>

class HttpResponse {
public:
    HttpResponse(): code(0), content() {}

    unsigned int code;
    std::string content;

    std::string to_string() const;
};

class IHttpServerHandler {
public:
    virtual ~IHttpServerHandler() {}
    virtual void handle_GET(const std::string &path, HttpResponse &response) = 0;
    virtual void handle_POST(const std::string &path, const std::string &data, HttpResponse &response) = 0;
};

class HttpServerImpl;

class HttpServer {
public:
    HttpServer(unsigned short port, IHttpServerHandler *handler);
    HttpServer(const HttpServer &) = delete;
    HttpServer &operator=(const HttpServer &) = delete;
    ~HttpServer();
    void service(bool wait);
private:
    HttpServerImpl *impl;
};

#endif
