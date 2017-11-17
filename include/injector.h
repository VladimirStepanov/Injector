#ifndef INJECTOR_H
#define INJECTOR_H

#include <string>
#include <unordered_map>

#include "exceptions.h"
#include "sock_bridge.h"

typedef int (*injector_cb)(SockBridge &obj);

const u_int16_t DEFAULT_PORT = 9876;

enum STATES {IN, OUT}; 

int DEF_CB(SockBridge &obj);

class InjectorException : public ExceptionBase {
public:
    InjectorException(std::string m, int ec) : ExceptionBase(m, ec) {}
};

class SocketError : public ExceptionBase {
public:
    SocketError(std::string m, int ec) : ExceptionBase(m, ec) {}
};

struct socks_info {
    int fd;
    bool status;
};



class Injector {
private:
    int master_socket, efd;
    std::unordered_map<int, socks_info*> s_fds;

    inline void set_non_block (int fd);
    inline int create_and_bind_s (u_int16_t port);
    inline void add_fds_to_map(int from_fd, int to_fd);
    inline void add_fd_to_map (int from, int to, bool state);
    inline void remove_fd_from_map (int fd);
    inline void add_sock_to_epoll (int fd);
    inline void add_fd_to_epoll (int fd);
    inline int connect_to_dst_host (struct sockaddr_in &addr);
    inline void handle_in(int local_fd, injector_cb in_cb, injector_cb out_cb);
    inline void close_connections(int local_fd);

public:
    Injector (u_int16_t port = DEFAULT_PORT);

    void loop (injector_cb in_cb, injector_cb out_cb = DEF_CB);

};

#endif