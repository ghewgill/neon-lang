#ifdef _WIN32
#include <winsock.h>
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int SOCKET;
#define closesocket(x) close(x)
#endif

#include "cell.h"

namespace rtl {

Cell net$tcp_socket()
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

Cell net$socket_accept(Cell &handle)
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

void net$socket_close(Cell &handle)
{
    SOCKET s = number_to_sint32(handle.number());
    closesocket(s);
}

void net$socket_connect(Cell &handle, const std::string &host, Number port)
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
    int r = connect(s, (sockaddr *)&sin, sizeof(sin));
    if (r < 0) {
        perror("connect");
    }
}

void net$socket_listen(Cell &handle, Number port)
{
    SOCKET s = number_to_sint32(handle.number());
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

std::string net$socket_recv(Cell &handle, Number count)
{
    SOCKET s = number_to_sint32(handle.number());
    int n = number_to_sint32(count);
    std::string buf(n, '\0');
    int r = recv(s, const_cast<char *>(buf.data()), n, 0);
    if (r < 0) {
        perror("recv");
        return "";
    }
    buf.resize(r);
    return buf;
}

void net$socket_send(Cell &handle, const std::string &data)
{
    SOCKET s = number_to_sint32(handle.number());
    send(s, data.data(), static_cast<int>(data.length()), 0);
}

}
