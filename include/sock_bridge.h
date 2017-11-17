#ifndef SOCK_BRIDGE_H
#define SOCK_BRIDGE_H

#include <sys/types.h>
#include "exceptions.h"

class SockBridgeException : public ExceptionBase {
public:
    SockBridgeException(std::string m, int ec) : ExceptionBase(m, ec) {}
};

class SockBridge {

private:
    int from, to;
public:
    SockBridge(int f, int t) : from(f), to(t) {}
    ssize_t read(u_int8_t *arr, ssize_t br);
    ssize_t write(u_int8_t *arr, ssize_t bw);

    int get_from_fd() const {return this->from;}
    int get_to_fd() const {return this->to;}
};


#endif