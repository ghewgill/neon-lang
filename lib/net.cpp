#include <errno.h>
#include <iso646.h>
#include <stdio.h>

#include "cell.h"
#include "rtl_exec.h"
#include "socketx.h"

namespace rtl {

namespace net {

Cell tcpSocket()
{
#ifdef _WIN32
    static bool initialized = false;
    if (!initialized) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(1, 1), &wsa);
        initialized = true;
    }
#endif
    Cell r;
    SOCKET s = socket(PF_INET, SOCK_STREAM, 0);
    r.array_index_for_write(0) = Cell(number_from_sint32(static_cast<int32_t>(s)));
    return r;
}

Cell socket_accept(Cell &handle)
{
    SOCKET s = number_to_sint32(handle.number());
    sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    SOCKET r = accept(s, reinterpret_cast<sockaddr *>(&sin), &slen);
    if (r < 0) {
        perror("accept");
        return Cell();
    }
    Cell client;
    client.array_index_for_write(0) = Cell(number_from_sint32(static_cast<int32_t>(r)));
    return client;
}

void socket_close(Cell &handle)
{
    SOCKET s = number_to_sint32(handle.number());
    closesocket(s);
}

void socket_connect(Cell &handle, const utf8string &host, Number port)
{
    SOCKET s = number_to_sint32(handle.number());
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

void socket_listen(Cell &handle, Number port)
{
    SOCKET s = number_to_sint32(handle.number());
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

std::vector<unsigned char> socket_recv(Cell &handle, Number count)
{
    SOCKET s = number_to_sint32(handle.number());
    int n = number_to_sint32(count);
    std::vector<unsigned char> buf(n);
    int r = recv(s, reinterpret_cast<char *>(const_cast<unsigned char *>(buf.data())), n, 0);
    if (r < 0) {
        perror("recv");
        return std::vector<unsigned char>();
    }
    buf.resize(r);
    return buf;
}

void socket_send(Cell &handle, const std::vector<unsigned char> &data)
{
    SOCKET s = number_to_sint32(handle.number());
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
    for (auto s: ra) {
        int fd = number_to_sint32(s.array_for_write()[0].number());
        FD_SET(fd, &rfds);
        if (fd+1 > nfds) {
            nfds = fd+1;
        }
    }

    std::vector<Cell> &wa = write->array_for_write();
    for (auto s: wa) {
        int fd = number_to_sint32(s.array_for_write()[0].number());
        FD_SET(fd, &wfds);
        if (fd+1 > nfds) {
            nfds = fd+1;
        }
    }

    std::vector<Cell> &ea = error->array_for_write();
    for (auto s: ea) {
        int fd = number_to_sint32(s.array_for_write()[0].number());
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
        if (FD_ISSET(number_to_sint32(i->array_for_write()[0].number()), &rfds)) {
            ++i;
        } else {
            ra.erase(i);
        }
    }

    for (auto i = wa.begin(); i != wa.end(); ) {
        if (FD_ISSET(number_to_sint32(i->array_for_write()[0].number()), &wfds)) {
            ++i;
        } else {
            wa.erase(i);
        }
    }

    for (auto i = ea.begin(); i != ea.end(); ) {
        if (FD_ISSET(number_to_sint32(i->array_for_write()[0].number()), &efds)) {
            ++i;
        } else {
            ea.erase(i);
        }
    }

    return true;
}

} // namespace net

} // namespace rtl
