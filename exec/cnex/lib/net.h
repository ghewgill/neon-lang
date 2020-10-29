#ifndef NET_H
#define NET_H

struct tagTExecutor;

void net_socket_tcpSocket(struct tagTExecutor *exec);
void net_socket_udpSocket(struct tagTExecutor *exec);

void net_socket_accept(struct tagTExecutor *exec);
void net_socket_bind(struct tagTExecutor *exec);
void net_socket_close(struct tagTExecutor *exec);
void net_socket_connect(struct tagTExecutor *exec);
void net_socket_listen(struct tagTExecutor *exec);
void net_socket_recv(struct tagTExecutor *exec);
void net_socket_send(struct tagTExecutor *exec);
void net_socket_select(struct tagTExecutor *exec);

#endif
