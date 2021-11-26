#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <assert.h>

#include "lib/socketx.h"
#include "util.h"

struct tagTExecutor;

typedef struct tagTHttpResponse {
    unsigned int code;
    struct tagTString *content;
} HttpResponse;

typedef struct tagTIHttpServerHandler {
    void (*handle_GET)(struct tagTExecutor *exec, struct tagTString *path, struct tagTHttpResponse *response);
    void (*handle_POST)(struct tagTExecutor *exec, struct tagTString *path, struct tagTString *data, struct tagTHttpResponse *response);
} IHttpServerHandler;

typedef struct tagTHttpServerImpl {
    struct tagTIHttpServerHandler *handler;
    struct tagTExecutor *exec;
    SOCKET server;
    size_t num_clients;
    struct tagTClient **clients;
} HttpServerImpl;

HttpServerImpl *server_createServer(unsigned short port, IHttpServerHandler *handler);
void server_service(HttpServerImpl *impl, BOOL wait);
void server_impl(HttpServerImpl *impl, BOOL wait);
void server_freeServer(HttpServerImpl *impl);

#endif
