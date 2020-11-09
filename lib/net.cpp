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
    virtual std::shared_ptr<SocketObject> accept() = 0;
    virtual void bind(const utf8string &address, Number port) = 0;
    virtual void close() = 0;
    virtual void connect(const utf8string &host, Number port) = 0;
    virtual utf8string getTlsVersion() { return utf8string(); }
    virtual void listen(Number port) = 0;
    virtual bool recv(Number count, std::vector<unsigned char> *buffer) = 0;
    virtual bool recvfrom(Number count, utf8string *remote_address, Number *remote_port, std::vector<unsigned char> *buffer) = 0;
    virtual void send(const std::vector<unsigned char> &data) = 0;
};

class RawSocketObject: public SocketObject {
    SOCKET handle;
public:
    explicit RawSocketObject(SOCKET handle): handle(handle) {}
    RawSocketObject(const RawSocketObject &) = delete;
    RawSocketObject &operator=(const RawSocketObject &) = delete;
    virtual ~RawSocketObject() {
        close();
    }
    virtual utf8string toString() const override { return utf8string("<RawSocket:" + std::to_string(handle) + ">"); }
    virtual bool is_valid() override { return handle != INVALID_SOCKET; }
    virtual SOCKET get_handle() override { return handle; }
    virtual std::shared_ptr<SocketObject> accept() override {
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
        if (handle != INVALID_SOCKET) {
            closesocket(handle);
            handle = INVALID_SOCKET;
        }
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

SSL_CTX *ctx = nullptr;

class TlsSocketObject: public SocketObject {
    std::shared_ptr<SocketObject> socket;
    SSL *ssl = nullptr;
    BIO *read_bio = nullptr;
    BIO *write_bio = nullptr;
    std::vector<unsigned char> write_buf;
    std::vector<unsigned char> encrypt_buf;
public:
    enum class TlsMode { CLIENT, SERVER };
    TlsSocketObject(std::shared_ptr<SocketObject> socket, TlsMode mode, const char *certfile, const char *keyfile): socket(socket) {
        if (ctx == nullptr) {
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            ctx = SSL_CTX_new(SSLv23_method());
            SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
            if (certfile != nullptr && keyfile != nullptr) {
                SSL_CTX_use_certificate_chain_file(ctx, certfile);
                SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM);
                if (SSL_CTX_check_private_key(ctx) != 1) {
                    abort();
                }
            }
        }
        ssl = SSL_new(ctx);
        read_bio = BIO_new(BIO_s_mem());
        write_bio = BIO_new(BIO_s_mem());
        switch (mode) {
            case TlsMode::CLIENT:
                SSL_set_connect_state(ssl);
                break;
            case TlsMode::SERVER:
                SSL_set_accept_state(ssl);
                break;
            default:
                abort();
        }
        SSL_set_bio(ssl, read_bio, write_bio);
    }
    TlsSocketObject(const TlsSocketObject &) = delete;
    TlsSocketObject &operator=(const TlsSocketObject &) = delete;
    virtual ~TlsSocketObject() {
        close();
        SSL_free(ssl);
    }
    virtual utf8string toString() const override { return utf8string("<TlsSocket:" + std::to_string(ctx != nullptr) + ">"); }
    virtual bool is_valid() override { return ctx != nullptr; }
    virtual SOCKET get_handle() override { return socket->get_handle(); }
    virtual std::shared_ptr<SocketObject> accept() override {
        return std::make_shared<TlsSocketObject>(socket->accept(), TlsMode::SERVER, nullptr, nullptr);
    }
    virtual void bind(const utf8string &address, Number port) override {
        socket->bind(address, port);
    }
    virtual void close() override {
        socket->close();
    }
    virtual void connect(const utf8string &host, Number port) override {
        socket->connect(host, port);
        SSL_set_tlsext_host_name(ssl, host.c_str());
        do_handshake();
    }
    virtual utf8string getTlsVersion() override {
        return utf8string(SSL_get_version(ssl));
    }
    virtual void listen(Number port) override {
        socket->listen(port);
    }
    virtual bool recv(Number count, std::vector<unsigned char> *buffer) override {
        //printf("recv\n");
        buffer->clear();
        std::vector<unsigned char> raw_buffer;
        bool r = socket->recv(count, &raw_buffer);
        if (not r) {
            //printf("  close\n");
            return false;
        }
        //printf("  raw_buffer=%zd\n", raw_buffer.size());
        size_t i = 0;
        while (i < raw_buffer.size()) {
            //printf("  i=%zd size=%zd\n", i, raw_buffer.size());
            int n = BIO_write(read_bio, raw_buffer.data() + i, raw_buffer.size() - i);
            if (n <= 0) {
                abort(); // TODO
            }
            i += n;
            if (not SSL_is_init_finished(ssl)) {
                //printf("  init not finished\n");
                do_handshake();
                if (not SSL_is_init_finished(ssl)) {
                    return true;
                }
                //printf("  init finished\n");
                process_outgoing_data();
            }
            for (;;) {
                unsigned char buf[1024];
                int r = SSL_read(ssl, buf, sizeof(buf));
                //printf("  r=%d\n", r);
                if (r <= 0) {
                    break;
                }
                std::copy(buf, buf + r, std::back_inserter(*buffer));
            }
            int err = SSL_get_error(ssl, n);
            err=err; //printf("  err=%d\n", err);
        }
        return true;
    }
    virtual bool recvfrom(Number, utf8string *, Number *, std::vector<unsigned char> *) override {
        return false;
    }
    virtual void send(const std::vector<unsigned char> &data) override {
        //printf("send %zd\n", data.size());
        std::copy(data.begin(), data.end(), std::back_inserter(encrypt_buf));
        process_outgoing_data();
    }
private:
    void do_handshake() {
        //printf("do_handshake\n");
        int r = SSL_do_handshake(ssl);
        //printf("  r=%d\n", r);
        int err = SSL_get_error(ssl, r);
        //printf("  err=%d\n", err);
        switch (err) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                process_send_data();
                break;
        }
        if (not write_buf.empty()) {
            //printf("  send write_buf %zd\n", write_buf.size());
            socket->send(write_buf);
            write_buf.clear();
        }
    }
    void process_outgoing_data() {
        //printf("process_outgoing_data\n");
        if (not SSL_is_init_finished(ssl)) {
            //printf("  waiting until init finished buffer=%zd\n", encrypt_buf.size());
            return;
        }
        //printf("  encrypting %zd\n", encrypt_buf.size());
        while (not encrypt_buf.empty()) {
            int n = SSL_write(ssl, encrypt_buf.data(), encrypt_buf.size());
            // TODO: check status
            if (n > 0) {
                encrypt_buf.erase(encrypt_buf.begin(), encrypt_buf.begin() + n);
                process_send_data();
            } else {
                break;
            }
        }
    }
    void process_send_data() {
        //printf("process_send_data\n");
        for (;;) {
            unsigned char buf[1024];
            int r = BIO_read(write_bio, buf, sizeof(buf));
            if (r > 0) {
                //printf("  copy %d to write_buf\n", r);
                std::copy(buf, buf+r, std::back_inserter(write_buf));
                fd_set fds;
                FD_ZERO(&fds);
                SOCKET h = socket->get_handle();
                FD_SET(h, &fds);
                if (select(h+1, NULL, &fds, NULL, 0) > 0) {
                    //printf("  actually sending write_buf\n");
                    socket->send(write_buf);
                    write_buf.clear();
                }
            } else if (not BIO_should_retry(write_bio)) {
                abort(); // TODO
            } else {
                break;
            }
        }
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
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    return std::make_shared<TlsSocketObject>(std::make_shared<RawSocketObject>(s), TlsSocketObject::TlsMode::CLIENT, nullptr, nullptr);
}

std::shared_ptr<Object> socket_tlsServerSocket(const utf8string &certfile, const utf8string &keyfile)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    return std::make_shared<TlsSocketObject>(std::make_shared<RawSocketObject>(s), TlsSocketObject::TlsMode::SERVER, certfile.c_str(), keyfile.c_str());
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

utf8string socket_getTlsVersion(const std::shared_ptr<Object> &socket)
{
    return check_socket(socket)->getTlsVersion();
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
