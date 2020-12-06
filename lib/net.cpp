#include <errno.h>
#include <iso646.h>
#include <stdio.h>

#include "cell.h"
#include "rtl_exec.h"
#include "socketx.h"

class SocketObject: public Object {
public:
    SOCKET handle;
public:
    explicit SocketObject(SOCKET handle): handle(handle) {}
    SocketObject(const SocketObject &) = delete;
    SocketObject &operator=(const SocketObject &) = delete;
    ~SocketObject() {
        if (handle != INVALID_SOCKET) {
            closesocket(handle);
        }
    }
    virtual utf8string toString() const { return utf8string("<Socket:" + std::to_string(handle) + ">"); }
};

static SocketObject *check_socket(const std::shared_ptr<Object> &ps)
{
    SocketObject *so = dynamic_cast<SocketObject *>(ps.get());
    if (so == nullptr || so->handle == INVALID_SOCKET) {
        throw RtlException(rtl::ne_net::Exception_SocketException, utf8string(""));
    }
    return so;
}

namespace rtl {

namespace ne_net {

#ifdef _WIN32
void initWinsock()
{
    static bool initialized = false;
    if (!initialized) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(1, 1), &wsa);
        initialized = true;
    }
}
#endif

std::shared_ptr<Object> socket_tcpSocket()
{
#ifdef _WIN32
    initWinsock();
#endif
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    return std::make_shared<SocketObject>(s);
}

std::shared_ptr<Object> socket_udpSocket()
{
#ifdef _WIN32
    initWinsock();
#endif
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    return std::make_shared<SocketObject>(s);
}

std::shared_ptr<Object> socket_accept(const std::shared_ptr<Object> &socket)
{
    SOCKET s = check_socket(socket)->handle;
    sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    SOCKET r = accept(s, reinterpret_cast<sockaddr *>(&sin), &slen);
    if (r < 0) {
        perror("accept");
        return nullptr;
    }
    return std::make_shared<SocketObject>(r);
}

void socket_bind(const std::shared_ptr<Object> &socket, const utf8string &address, Number port)
{
    SOCKET s = check_socket(socket)->handle;
    int p = number_to_sint32(port);
    in_addr addr;
    if (address.empty()) {
        addr.s_addr = INADDR_ANY;
    } else {
        addr.s_addr = inet_addr(address.c_str());
        if (addr.s_addr == INADDR_NONE) {
            struct hostent *he = gethostbyname(address.c_str());
            if (he == NULL) {
                return;
            }
            addr = *reinterpret_cast<in_addr *>(he->h_addr);
        }
    }
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = addr;
    sin.sin_port = htons(static_cast<uint16_t>(p));
    int r = bind(s, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
    if (r < 0) {
        perror("bind");
    }
}

void socket_close(const std::shared_ptr<Object> &socket)
{
    SocketObject *s = check_socket(socket);
    closesocket(s->handle);
    s->handle = -1;
}

void socket_connect(const std::shared_ptr<Object> &socket, const utf8string &host, Number port)
{
    SOCKET s = check_socket(socket)->handle;
    int p = number_to_sint32(port);
    in_addr addr;
    addr.s_addr = inet_addr(host.c_str());
    if (addr.s_addr == INADDR_NONE) {
        struct hostent *he = gethostbyname(host.c_str());
        if (he == NULL) {
            return;
        }
        addr = *reinterpret_cast<in_addr *>(he->h_addr);
    }
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = addr;
    sin.sin_port = htons(static_cast<uint16_t>(p));
    int r = connect(s, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
    if (r < 0) {
        perror("connect");
    }
}

void socket_listen(const std::shared_ptr<Object> &socket, Number port)
{
    SOCKET s = check_socket(socket)->handle;
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&on), sizeof(on));
    int p = number_to_sint32(port);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(static_cast<uint16_t>(p));
    int r = bind(s, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
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

bool socket_recv(const std::shared_ptr<Object> &socket, Number count, std::vector<unsigned char> *buffer)
{
    SOCKET s = check_socket(socket)->handle;
    int n = number_to_sint32(count);
    buffer->resize(n);
    int r = recv(s, reinterpret_cast<char *>(const_cast<unsigned char *>(buffer->data())), n, 0);
    if (r < 0) {
        perror("recv");
        buffer->clear();
        return false;
    }
    if (r == 0) {
        buffer->clear();
        return false;
    }
    buffer->resize(r);
    return true;
}

bool socket_recvfrom(const std::shared_ptr<Object> &socket, Number count, utf8string *remote_address, Number *remote_port, std::vector<unsigned char> *buffer)
{
    SOCKET s = check_socket(socket)->handle;
    int n = number_to_sint32(count);
    buffer->resize(n);
    struct sockaddr_in sin;
    socklen_t sin_len = sizeof(sin);
    int r = recvfrom(s, reinterpret_cast<char *>(const_cast<unsigned char *>(buffer->data())), n, 0, reinterpret_cast<sockaddr *>(&sin), &sin_len);
    if (r < 0) {
        perror("recvfrom");
        buffer->clear();
        return false;
    }
    if (r == 0) {
        buffer->clear();
        return false;
    }
    buffer->resize(r);
    *remote_address = utf8string(inet_ntoa(sin.sin_addr));
    *remote_port = number_from_uint32(ntohs(sin.sin_port));
    return true;
}

void socket_send(const std::shared_ptr<Object> &socket, const std::vector<unsigned char> &data)
{
    SOCKET s = check_socket(socket)->handle;
    send(s, reinterpret_cast<const char *>(data.data()), static_cast<int>(data.size()), 0);
}

bool socket_select(Cell *read, Cell *write, Cell *error, Number timeout_seconds)
{
    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    int nfds = 0;

    std::vector<Cell> &ra = read->array_for_write();
    for (auto &s: ra) {
        int fd = check_socket(s.array_index_for_read(0).object())->handle;
        FD_SET(fd, &rfds);
        if (fd+1 > nfds) {
            nfds = fd+1;
        }
    }

    std::vector<Cell> &wa = write->array_for_write();
    for (auto &s: wa) {
        int fd = check_socket(s.array_index_for_read(0).object())->handle;
        FD_SET(fd, &wfds);
        if (fd+1 > nfds) {
            nfds = fd+1;
        }
    }

    std::vector<Cell> &ea = error->array_for_write();
    for (auto &s: ea) {
        int fd = check_socket(s.array_index_for_read(0).object())->handle;
        FD_SET(fd, &efds);
        if (fd+1 > nfds) {
            nfds = fd+1;
        }
    }

    struct timeval actual_tv;
    struct timeval *tv = NULL;
    if (not number_is_negative(timeout_seconds)) {
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
    } while (false);
    if (r < 0) {
        throw RtlException(Exception_SocketException, utf8string(""));
    }
    if (r == 0) {
        ra.clear();
        wa.clear();
        ea.clear();
        return false;
    }

    for (auto i = ra.begin(); i != ra.end(); ) {
        int fd = check_socket(i->array_index_for_read(0).object())->handle;
        if (FD_ISSET(fd, &rfds)) {
            ++i;
        } else {
            i = ra.erase(i);
        }
    }

    for (auto i = wa.begin(); i != wa.end(); ) {
        int fd = check_socket(i->array_index_for_read(0).object())->handle;
        if (FD_ISSET(fd, &wfds)) {
            ++i;
        } else {
            i = wa.erase(i);
        }
    }

    for (auto i = ea.begin(); i != ea.end(); ) {
        int fd = check_socket(i->array_index_for_read(0).object())->handle;
        if (FD_ISSET(fd, &efds)) {
            ++i;
        } else {
            i = ea.erase(i);
        }
    }

    return true;
}

} // namespace ne_net

} // namespace rtl
