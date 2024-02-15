// Setup the network sockets for the different platforms.
#ifndef SOCKETX_H
#define SOCKETX_H
#ifdef _WIN32

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib") // Include the wsock32 (version 2) library, automatically on Windows builds.
typedef int socklen_t;
#pragma warning(disable: 4127) // incompatible with FD_SET()
#define socket_error()    WSAGetLastError()

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define closesocket(x)      close(x)
#define INVALID_SOCKET     -1
#define socket_error()      errno

typedef int SOCKET;

#endif
#endif
