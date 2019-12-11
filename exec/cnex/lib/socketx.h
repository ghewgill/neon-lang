// Setup the network sockets for the different platforms.
#ifdef _WIN32

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib") // Include the wsock32 (version 2) library, automatically on Windows builds.
typedef int socklen_t;
#pragma warning(disable: 4127) // incompatible with FD_SET()

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define closesocket(x)      close(x)

typedef int SOCKET;
const int INVALID_SOCKET = -1;

#endif
