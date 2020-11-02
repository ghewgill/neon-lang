#include "net.h"

// Needed for gethostbyname() on some Linux platforms.
#define _DEFAULT_SOURCE
#include <assert.h>
#include <errno.h>
#include <iso646.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "object.h"
#include "socketx.h"
#include "stack.h"
#include "nstring.h"


void object_releaseSocketObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                SOCKET s = *(SOCKET*)o->ptr;
                if (s != INVALID_SOCKET) {
                    closesocket(s);
                }
            }
            free(o->ptr);
            free(o);
        }
    }
}

Object *object_createSocketObject(SOCKET s)
{
    Object *r = object_createObject();
    r->ptr = malloc(sizeof(SOCKET));
    *(SOCKET *)r->ptr = s;
    r->release = object_releaseSocketObject;

    return r;
}


static SOCKET *check_socket(TExecutor *exec, Object *o)
{
    // TODO: How can this function tell whether this is actually a socket object?
    if (o == NULL || o->ptr == NULL || *(SOCKET *)o->ptr == INVALID_SOCKET) {
        exec->rtl_raise(exec, "SocketException", "invalid socket");
        return NULL;
    }
    return o->ptr;
}


#ifdef _WIN32
static void initWinsock()
{
    static BOOL initialized = FALSE;
    if (!initialized) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(1, 1), &wsa);
        initialized = TRUE;
    }
}
#endif

void net_socket_tcpSocket(TExecutor *exec)
{
#ifdef _WIN32
    initWinsock();
#endif
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    push(exec->stack, cell_fromObject(object_createSocketObject(s)));
}

void net_socket_udpSocket(TExecutor *exec)
{
#ifdef _WIN32
    initWinsock();
#endif
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    push(exec->stack, cell_fromObject(object_createSocketObject(s)));
}

void net_socket_accept(TExecutor *exec)
{
    Object *o = top(exec->stack)->object; pop(exec->stack);
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    SOCKET r = accept(s, (struct sockaddr *)(&sin), &slen);
    if (r < 0) {
        perror("accept");
        push(exec->stack, cell_newCell());
        return;
    }
    push(exec->stack, cell_fromObject(object_createSocketObject(r)));
}

void net_socket_bind(TExecutor *exec)
{
    Number port = peek(exec->stack, 0)->number;
    const char *address = string_ensureNullTerminated(peek(exec->stack, 1)->string);
    Object *o = peek(exec->stack, 2)->object;
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    int p = number_to_sint32(port);
    struct in_addr addr;
    if (strlen(address) == 0) {
        addr.s_addr = INADDR_ANY;
    } else {
        addr.s_addr = inet_addr(address);
        if (addr.s_addr == INADDR_NONE) {
            struct hostent *he = gethostbyname(address);
            if (he == NULL) {
                // ToDo: Raise exception here, that host does not exist?
                goto bail;
            }
            addr = *(struct in_addr*)he->h_addr_list[0];
        }
    }
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = addr;
    sin.sin_port = htons((uint16_t)p);
    int r = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (r < 0) {
        // ToDo: Raise exception here?
        perror("bind");
    }

bail:
    pop(exec->stack);
    pop(exec->stack);
    pop(exec->stack);
}

void net_socket_close(TExecutor *exec)
{
    Object *o = top(exec->stack)->object; pop(exec->stack);
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    closesocket(s);
    *ps = INVALID_SOCKET;
}

void net_socket_connect(TExecutor *exec)
{
    Number port = peek(exec->stack, 0)->number;
    const char *host = string_ensureNullTerminated(peek(exec->stack, 1)->string);
    Object *o = peek(exec->stack, 2)->object;
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    int p = number_to_sint32(port);
    struct in_addr addr;
    addr.s_addr = inet_addr(host);
    if (addr.s_addr == INADDR_NONE) {
        struct hostent *he = gethostbyname(host);
        if (he == NULL) {
            // ToDo: Shouldn't we raise an exception here so the user is at least aware that the connection failed?
            //exec->rtl_raise(exec, "SocketException", "unknown host", number_from_sint32(WSAGetLastError()));
            goto bail;
        }
        addr = *(struct in_addr *)he->h_addr_list[0];
    }
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = addr;
    sin.sin_port = htons((uint16_t)p);
    int r = connect(s, (struct sockaddr *)&sin, sizeof(sin)); 
    if (r < 0) {
        // ToDo: Raise exception here?
        perror("connect");
    }

bail:
    pop(exec->stack);
    pop(exec->stack);
    pop(exec->stack);
}

void net_socket_listen(TExecutor *exec)
{
    Number port = top(exec->stack)->number; pop(exec->stack);
    Object *o = top(exec->stack)->object; pop(exec->stack);
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    int p = number_to_sint32(port);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((uint16_t)p);
    int r = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (r < 0) {
        perror("bind");
        return;
    }
    r = listen(s, 5);
    if (r < 0) {
        perror("listen");
        return;
    }
}

void net_socket_recv(TExecutor *exec)
{
    Number count = top(exec->stack)->number; pop(exec->stack);
    Object *o = top(exec->stack)->object; pop(exec->stack);
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    int n = number_to_sint32(count);
    unsigned char *buf = malloc(n);
    if (buf == NULL) {
        fatal_error("Could not allocate %d bytes for socket_recv().", n);
    }
    int r = recv(s, (char*)buf, n, 0);
    if (r < 0) {
        perror("recv");
        Cell *empty = cell_fromCString("");
        // We need to make sure that we're returning a BYTES cell.
        empty->type = cBytes;
        push(exec->stack, empty);
    }
    if (r == 0) {
        push(exec->stack, cell_fromBoolean(FALSE));
        push(exec->stack, cell_fromBytes(string_newString()));
        return;
    }
    TString *ret = string_newString();
    ret->data = (char*)buf;
    ret->length = r;

    // Note that cell_fromBytes() will automatically resize (truncate) the returned buffer to r.
    push(exec->stack, cell_fromBoolean(TRUE));
    push(exec->stack, cell_fromBytes(ret));
    string_freeString(ret);
}

void net_socket_send(TExecutor *exec)
{
    TString *data = top(exec->stack)->string;
    Object *o = peek(exec->stack, 1)->object;
    SOCKET *ps = check_socket(exec, o);
    if (ps == NULL) {
        return;
    }
    SOCKET s = *ps;

    send(s, data->data, (int)data->length, 0);

    pop(exec->stack);
    pop(exec->stack);
}

void net_socket_select(TExecutor *exec)
{
    Number timeout_seconds = top(exec->stack)->number; pop(exec->stack);
    Cell *error = peek(exec->stack, 0)->address;
    Cell *write = peek(exec->stack, 1)->address;
    Cell *read = peek(exec->stack, 2)->address;

    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    int nfds = 0;

    for (size_t i = 0; i < read->array->size; i++) {
        SOCKET *ps = check_socket(exec, read->array->data[i].object);
        SOCKET fd = *ps;
        FD_SET(fd, &rfds);
        if (fd + 1 > nfds) {
            nfds = fd+1;
        }
    }

    for (size_t i = 0; i < write->array->size; i++) {
        SOCKET *ps = check_socket(exec, read->array->data[i].object);
        SOCKET fd = *ps;
        FD_SET(fd, &wfds);
        if (fd + 1 > nfds) {
            nfds = fd+1;
        }
    }

    for (size_t i = 0; i < error->array->size; i++) {
        SOCKET *ps = check_socket(exec, read->array->data[i].object);
        SOCKET fd = *ps;
        FD_SET(fd, &efds);
        if (fd + 1 > nfds) {
            nfds = fd+1;
        }
    }

    struct timeval actual_tv;
    struct timeval *tv = NULL;
    if (!number_is_negative(timeout_seconds)) {
        actual_tv.tv_sec = number_to_sint32(number_trunc(timeout_seconds));
        actual_tv.tv_usec = number_to_sint32(number_modulo(number_multiply(timeout_seconds, number_from_sint32(1000000)), number_from_sint32(1000000)));
        tv = &actual_tv;
    }
    int r;
    do {
        r = select(nfds, &rfds, &wfds, &efds, tv);
        if (r < 0 && errno == EAGAIN) {
            continue;
        }
    } while (FALSE);

    if (r < 0) {
        exec->rtl_raise(exec, "SocketException", number_to_string(number_from_sint32(errno)));
        return;
    }

    if (r == 0) {
        array_clearArray(read->array);
        array_clearArray(write->array);
        array_clearArray(error->array);
        push(exec->stack, cell_fromBoolean(FALSE));
        return;
    }

    for (size_t i = 0; i < read->array->size; ) {
        SOCKET *ps = check_socket(exec, read->array->data[i].object);
        SOCKET fd = *ps;
        if (FD_ISSET(fd, &rfds)) {
            ++i;
        } else {
            array_removeItem(read->array, i);
        }
    }

    for (size_t i = 0; i < write->array->size; ) {
        SOCKET *ps = check_socket(exec, write->array->data[i].object);
        SOCKET fd = *ps;
        if (FD_ISSET(fd, &rfds)) {
            ++i;
        } else {
            array_removeItem(write->array, i);
        }
    }

    for (size_t i = 0; i < error->array->size; ) {
        SOCKET *ps = check_socket(exec, error->array->data[i].object);
        SOCKET fd = *ps;
        if (FD_ISSET(fd, &rfds)) {
            ++i;
        } else {
            array_removeItem(error->array, i);
        }
    }

    push(exec->stack, cell_fromBoolean(TRUE));
}
