#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "sock_bridge.h"


ssize_t SockBridge::read(u_int8_t *arr, ssize_t br) {

    ssize_t res = recv(this->from, arr, br, 0);
    if (res == -1)
        throw SockBridgeException("recv error", errno);

    return res;
}

ssize_t SockBridge::write(u_int8_t *arr, ssize_t bw) {

    ssize_t res = send(this->to, arr, bw, 0);
    if (res == -1)
        throw SockBridgeException("send error", errno);

    return res;
}

