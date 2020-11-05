#include <errno.h>
#include <iso646.h>
#include <stdio.h>

#include <openssl/ssl.h>

#include "cell.h"
#include "rtl_exec.h"
#include "socketx.h"

// https://wiki.openssl.org/index.php/SSL/TLS_Client

class SocketObject: public Object {
public:
    SocketObject() {}
    SocketObject(const SocketObject &) = delete;
    SocketObject &operator=(const SocketObject &) = delete;
    virtual ~SocketObject() {}
    virtual bool is_valid() = 0;
    virtual SOCKET get_handle() = 0;
    virtual std::shared_ptr<Object> accept() = 0; 
    virtual void bind(const utf8string &address, Number port) = 0;
    virtual void close() = 0;
    virtual void connect(const utf8string &host, Number port) = 0;
    virtual void listen(Number port) = 0;
    virtual bool recv(Number count, std::vector<unsigned char> *buffer) = 0;
    virtual bool recvfrom(Number count, utf8string *remote_address, Number *remote_port, std::vector<unsigned char> *buffer) = 0;
    virtual void send(const std::vector<unsigned char> &data) = 0;
};

class RawSocketObject: public SocketObject {
public:
    SOCKET handle;
public:
    explicit RawSocketObject(SOCKET handle): handle(handle) {}
    RawSocketObject(const RawSocketObject &) = delete;
    RawSocketObject &operator=(const RawSocketObject &) = delete;
    virtual ~RawSocketObject() {
        if (handle != INVALID_SOCKET) {
            closesocket(handle);
        }
    }
    virtual utf8string toString() const override { return utf8string("<RawSocket:" + std::to_string(handle) + ">"); }
    virtual bool is_valid() override { return handle != INVALID_SOCKET; }
    virtual SOCKET get_handle() override { return handle; }
    virtual std::shared_ptr<Object> accept() override {
        sockaddr_in sin;
        socklen_t slen = sizeof(sin);
        SOCKET r = ::accept(handle, reinterpret_cast<sockaddr *>(&sin), &slen);
        if (r < 0) {
            perror("accept");
            return nullptr;
        }
        return std::make_shared<RawSocketObject>(r);
    }
    virtual void bind(const utf8string &address, Number port) override {
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
        int r = ::bind(handle, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
        if (r < 0) {
            perror("bind");
        }
    }
    virtual void close() override {
        closesocket(handle);
        handle = -1;
    }
    virtual void connect(const utf8string &host, Number port) override {
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
        int r = ::connect(handle, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
        if (r < 0) {
            perror("connect");
        }
    }
    virtual void listen(Number port) override {
        int on = 1;
        setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&on), sizeof(on));
        int p = number_to_sint32(port);
        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(static_cast<uint16_t>(p));
        int r = ::bind(handle, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
        if (r < 0) {
            perror("bind");
            return;
        }
        r = ::listen(handle, 5);
        if (r < 0) {
            perror("listen");
            return;
        }
    }
    virtual bool recv(Number count, std::vector<unsigned char> *buffer) override {
        int n = number_to_sint32(count);
        buffer->resize(n);
        int r = ::recv(handle, reinterpret_cast<char *>(const_cast<unsigned char *>(buffer->data())), n, 0);
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
    virtual bool recvfrom(Number count, utf8string *remote_address, Number *remote_port, std::vector<unsigned char> *buffer) override {
        int n = number_to_sint32(count);
        buffer->resize(n);
        struct sockaddr_in sin;
        socklen_t sin_len = sizeof(sin);
        int r = ::recvfrom(handle, reinterpret_cast<char *>(const_cast<unsigned char *>(buffer->data())), n, 0, reinterpret_cast<sockaddr *>(&sin), &sin_len);
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
    virtual void send(const std::vector<unsigned char> &data) override {
        ::send(handle, reinterpret_cast<const char *>(data.data()), static_cast<int>(data.size()), 0);
    }
};

class TlsClientObject: public SocketObject {
public:
    SSL_CTX *ctx = nullptr;
    BIO *bio = nullptr;
    SSL *ssl = nullptr;
public:
    TlsClientObject() {}
    TlsClientObject(const TlsClientObject &) = delete;
    TlsClientObject &operator=(const TlsClientObject &) = delete;
    virtual ~TlsClientObject() {
    }
    virtual utf8string toString() const override { return utf8string("<TlsSocket:" + std::to_string(ctx != nullptr) + ">"); }
    virtual bool is_valid() override { return ctx != nullptr; }
    virtual SOCKET get_handle() override { return INVALID_SOCKET; }
    virtual std::shared_ptr<Object> accept() override {
        return nullptr;
    }
    virtual void bind(const utf8string &, Number) override {
    }
    virtual void close() override {
    }
    virtual void connect(const utf8string &, Number) override {
    }
    virtual void listen(Number) override {
    }
    virtual bool recv(Number, std::vector<unsigned char> *) override {
        return false;
    }
    virtual bool recvfrom(Number, utf8string *, Number *, std::vector<unsigned char> *) override {
        return false;
    }
    virtual void send(const std::vector<unsigned char> &) override {
    }
};

static SocketObject *check_socket(const std::shared_ptr<Object> &ps)
{
    SocketObject *so = dynamic_cast<SocketObject *>(ps.get());
    if (so == nullptr || not so->is_valid()) {
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
    return std::make_shared<RawSocketObject>(s);
}

std::shared_ptr<Object> socket_tlsClientSocket()
{
    return std::make_shared<TlsClientObject>();
}

std::shared_ptr<Object> socket_udpSocket()
{
#ifdef _WIN32
    initWinsock();
#endif
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    return std::make_shared<RawSocketObject>(s);
}

std::shared_ptr<Object> socket_accept(const std::shared_ptr<Object> &socket)
{
    return check_socket(socket)->accept();
}

void socket_bind(const std::shared_ptr<Object> &socket, const utf8string &address, Number port)
{
    check_socket(socket)->bind(address, port);
}

void socket_close(const std::shared_ptr<Object> &socket)
{
    check_socket(socket)->close();
}

void socket_connect(const std::shared_ptr<Object> &socket, const utf8string &host, Number port)
{
    check_socket(socket)->connect(host, port);
}

void socket_listen(const std::shared_ptr<Object> &socket, Number port)
{
    check_socket(socket)->listen(port);
}

bool socket_recv(const std::shared_ptr<Object> &socket, Number count, std::vector<unsigned char> *buffer)
{
    return check_socket(socket)->recv(count, buffer);
}

bool socket_recvfrom(const std::shared_ptr<Object> &socket, Number count, utf8string *remote_address, Number *remote_port, std::vector<unsigned char> *buffer)
{
    return check_socket(socket)->recvfrom(count, remote_address, remote_port, buffer);
}

void socket_send(const std::shared_ptr<Object> &socket, const std::vector<unsigned char> &data)
{
    check_socket(socket)->send(data);
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
        SOCKET fd = check_socket(s.object())->get_handle();
        if (fd != INVALID_SOCKET) {
            FD_SET(fd, &rfds);
            if (fd+1 > nfds) {
                nfds = fd+1;
            }
        }
    }

    std::vector<Cell> &wa = write->array_for_write();
    for (auto &s: wa) {
        SOCKET fd = check_socket(s.object())->get_handle();
        if (fd != INVALID_SOCKET) {
            FD_SET(fd, &wfds);
            if (fd+1 > nfds) {
                nfds = fd+1;
            }
        }
    }

    std::vector<Cell> &ea = error->array_for_write();
    for (auto &s: ea) {
        SOCKET fd = check_socket(s.object())->get_handle();
        if (fd != INVALID_SOCKET) {
            FD_SET(fd, &efds);
            if (fd+1 > nfds) {
                nfds = fd+1;
            }
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
        SOCKET fd = check_socket(i->object())->get_handle();
        if (fd != INVALID_SOCKET) {
            if (FD_ISSET(fd, &rfds)) {
                ++i;
            } else {
                i = ra.erase(i);
            }
        }
    }

    for (auto i = wa.begin(); i != wa.end(); ) {
        SOCKET fd = check_socket(i->object())->get_handle();
        if (fd != INVALID_SOCKET) {
            if (FD_ISSET(fd, &wfds)) {
                ++i;
            } else {
                i = wa.erase(i);
            }
        }
    }

    for (auto i = ea.begin(); i != ea.end(); ) {
        SOCKET fd = check_socket(i->object())->get_handle();
        if (fd != INVALID_SOCKET) {
            if (FD_ISSET(fd, &efds)) {
                ++i;
            } else {
                i = ea.erase(i);
            }
        }
    }

    return true;
}

} // namespace ne_net

} // namespace rtl
