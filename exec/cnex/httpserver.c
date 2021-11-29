#include "httpserver.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "lib/socketx.h"

static const int POST_MAX_LENGTH = 10000000;



HttpResponse *response_createResponse()
{
    HttpResponse *r = malloc(sizeof(HttpResponse));
    r->code = 0;
    r->content = NULL;
    return r;
}

TString *response_toString(HttpResponse *r)
{
    char buff[64];
    if (r->code == 204 || r->code == 404) {
        snprintf(buff, sizeof(buff), "HTTP/1.0 %d ok\r\n", r->code);
    } else {
        snprintf(buff, sizeof(buff), "HTTP/1.0 %d ok\r\nContent-length: %zu\r\n\r\n", r->code, r->content->length);
    }
    TString *s = string_createCString(buff);
    assert(r->content != NULL);
    s = string_appendString(s, r->content);
    return s;
}

void response_freeResponse(HttpResponse *r)
{
    string_freeString(r->content);
    free(r);
}



typedef struct tagTClient {
    struct tagTExecutor *exec;
    IHttpServerHandler *handler;
    SOCKET socket;
    TString *request;
    TString *path;
    int64_t post_length;
    TString *post_data;
} Client;

Client *client_createClient(struct tagTExecutor *exec, struct tagTIHttpServerHandler *handler, SOCKET socket)
{
    Client *c = malloc(sizeof(Client));
    c->handler = handler;
    c->socket = socket;
    c->request = NULL;
    c->path = NULL;
    c->post_length = -1;
    c->post_data = NULL;
    c->exec = exec;
    return c;
}

void client_handleResponse(struct tagTClient *c, struct tagTHttpResponse *response)
{
    if (response->code == 0) {
        // If the code is 0, that means there is no body, nothing to return; so set the code to 204 (No Content)
        // and respond with it.
        response->code = 204;
    }
    TString *r = response_toString(response);
    send(c->socket, r->data, (int)r->length, 0);
    string_freeString(r);
}

BOOL client_headerMatch(TString *header, const char *target, TString **value)
{
    TString *t = string_createCString(target);
    TString *h = string_subString(header, 0, t->length);

    // The following will convert the strings to lowercase in place, rather than creating a whole new string
    // in the process like string_toLowerCase() does.  (There's no reason to do that.)
    for (size_t c = 0; c < h->length; c++) {
        h->data[c] = (char)tolower(h->data[c]);
    }
    for (size_t c = 0; c < t->length; c++) {
        t->data[c] = (char)tolower(t->data[c]);
    }
    if (string_compareString(h, t) == 0) {
        size_t i = t->length;
        size_t c = 0;
        while (i < header->length && isspace(header->data[i])) {
            ++i;
            ++c;
        }
        *value = string_subString(header, i, header->length - i);
        string_freeString(h);
        string_freeString(t);
        return TRUE;
    }
    string_freeString(h);
    string_freeString(t);
    return FALSE;
}

BOOL client_handleRequest(struct tagTClient *c)
{
    BOOL retval = FALSE;
    TStringArray *lines = string_createStringArray();
    uint64_t i = 0;
    for (;;) {
        uint64_t start = i;
        i = string_findCString(c->request, i, "\r\n");
        if (i == NPOS || i == start) {
            break;
        }
        string_appendStringArrayElement(lines, string_subString(c->request, start, i - start));
        i += 2;
    }
    if (lines->size == 0) {
        string_freeStringArray(lines);
        return FALSE;
    }
    TStringArray *req = string_splitString(lines->data[0], ' ');
    HttpResponse *response = response_createResponse();
    if (string_compareCString(req->data[0], "GET") == 0) {
        c->handler->handle_GET(c->exec, req->data[1], response);
    } else if (string_compareCString(req->data[0], "POST") == 0) {
        if (req->size >= 2) {
            // We found the path, so store that in the client object.
            c->path = string_fromString(req->data[1]);
        }
        for (int idx = 0; idx < lines->size; idx++) {
            // Iterate the headers, looking for the headers we actually care about.
            TString *value = NULL;
            TString *h = lines->data[idx];
            // ToDo: Should we consider searching for Content-Type: text/html; charset=utf-8:
            // to ensure that it is ASCII or UTF8?  (This might be more revelant when we become a debug server adapter.)
            if (client_headerMatch(h, "Content-Length:", &value)) {
                int64_t length = atoi(string_ensureNullTerminated(value));
                string_freeString(value);
                if (length >= 0 && length <= POST_MAX_LENGTH) {
                    c->post_length = length;
                    c->post_length -= (int)c->post_data->length;
                    if (c->post_length > 0) {
                        retval = FALSE;
                        goto cleanup;
                    } else {
                        c->handler->handle_POST(c->exec, req->data[1], c->post_data, response);
                    }
                }
                break;
            }
            string_freeString(value);
        }
    } else {
        goto cleanup;
    }
    client_handleResponse(c, response);
    retval = TRUE;

cleanup:
    string_freeStringArray(lines);
    string_freeStringArray(req);
    response_freeResponse(response);
    return retval;
}

BOOL client_handleData(struct tagTClient *c, TString *data)
{
    if (c->post_length < 0) {
        if (c->request == NULL) {
            c->request = string_fromString(data);
        } else {
            c->request = string_appendString(c->request, data);
        }
        uint64_t end = string_findCString(c->request, 0, "\r\n\r\n");
        if (end != NPOS) {
            end += 4;
            c->post_length = c->request->length - end;
            c->post_data = string_subString(c->request, end, c->post_length);
            if (client_handleRequest(c)) {
                return TRUE;
            }
        }
    } else {
        c->post_data = string_appendString(c->post_data, data);
        c->post_length -= (int)data->length;
        if (c->post_length <= 0) {
            HttpResponse *httpresponse = response_createResponse();
            c->handler->handle_POST(c->exec, c->path, c->post_data, httpresponse);
            client_handleResponse(c, httpresponse);
            response_freeResponse(httpresponse);
            return TRUE;
        }
    }
    return FALSE;
}

void client_freeClient(struct tagTClient *c)
{
    closesocket(c->socket);
    string_freeString(c->request);
    string_freeString(c->post_data);
    string_freeString(c->path);
    free(c);
    c = NULL;
}



HttpServerImpl *server_createServer(unsigned short port, IHttpServerHandler *handler)
{
    HttpServerImpl *server = malloc(sizeof(HttpServerImpl));
    server->handler = handler;
    server->clients = NULL;
    server->num_clients = 0;

#ifdef _WIN32
    static BOOL initialized = FALSE;
    if (!initialized) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(1, 1), &wsa);
        initialized = TRUE;
    }
#endif
    server->server = socket(PF_INET, SOCK_STREAM, 0);
    assert(server->server != INVALID_SOCKET);
    int on = 0;
    int r = setsockopt(server->server, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    assert(r == 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    r = bind(server->server, (struct sockaddr *)&sin, sizeof(sin));
    if (r != 0) {
        fatal_error("Could not bind debug server port %d.  Try choosing a different port?", port);
    }
    r = listen(server->server, 5);
    assert(r == 0);
    return server;
}

void server_removeClient(HttpServerImpl *self, size_t index)
{
    Client *c = self->clients[index];
    for(size_t i = index; i+1 < self->num_clients; i++) {
        self->clients[i] = self->clients[i+1];
    }
    self->num_clients--;
    self->clients = realloc(self->clients, (sizeof(Client*) * self->num_clients));
    client_freeClient(c);
}

void server_service(HttpServerImpl *impl, BOOL wait)
{
    server_impl(impl, wait);
}

void server_freeServer(HttpServerImpl *impl)
{
    if (impl == NULL) {
        return;
    }
    closesocket(impl->server);
    for (size_t i = 0; i < impl->num_clients; i++) {
        client_freeClient(impl->clients[i]);
    }
    free(impl->clients);
    free(impl);
}


void server_impl(HttpServerImpl *impl, BOOL wait)
{
    if (impl->server == INVALID_SOCKET) {
        return;
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(impl->server, &rfds);
    SOCKET maxsocket = impl->server;
    for (size_t i = 0; i < impl->num_clients; i++) {
        FD_SET(impl->clients[i]->socket, &rfds);
        if (impl->clients[i]->socket > maxsocket) {
            maxsocket = impl->clients[i]->socket;
        }
    }
    struct timeval tv = {0, 0};
    int n = select((int)maxsocket+1, &rfds, NULL, NULL, wait ? NULL : &tv);
    if (n > 0) {
        if (FD_ISSET(impl->server, &rfds)) {
            struct sockaddr_in sin;
            socklen_t socklen = sizeof(sin);
            SOCKET t = accept(impl->server, (struct sockaddr *)&sin, &socklen);
            assert(t != INVALID_SOCKET);
            Client *c = client_createClient(impl->exec, impl->handler, t);
            if (impl->clients) {
                impl->clients = realloc(impl->clients, sizeof(Client*) * (impl->num_clients + 1));
                impl->num_clients++;
            }
            if (impl->clients == NULL) {
                impl->clients = malloc(sizeof(Client*));
                impl->num_clients = 1;
            }
            impl->clients[impl->num_clients-1] = c;
        }
        for (size_t i = 0; i < impl->num_clients; i++) {
            Client *c = impl->clients[i];
            if (FD_ISSET(c->socket, &rfds)) {
                char buf[200];
                n = recv(c->socket, buf, sizeof(buf), 0);
                if (n > 0) {
                    TString *s = string_createStringFromData(buf, n);
                    if (client_handleData(c, s)) {
                        server_removeClient(impl, i);
                    } else {
                        string_freeString(s);
                        continue;
                    }
                    string_freeString(s);
                } else {
                    server_removeClient(impl, i);
                }
            } else {
                continue;
            }
        }
    }
}

void server_freeService(HttpServerImpl *impl)
{
    free(impl->handler);
    closesocket(impl->server);
    free(impl->clients);
    free(impl);
}
