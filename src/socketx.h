#ifdef _WIN32

#include <winsock.h>
typedef int socklen_t;
#pragma warning(disable: 4127) // incompatible with FD_SET()
#define socket_error()  WSAGetLastError()

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int SOCKET;
const int INVALID_SOCKET = -1;
inline void closesocket(int x) { close(x); }
#define socket_error()   errno

#endif
