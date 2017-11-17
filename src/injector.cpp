#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include <arpa/inet.h>
#include <unordered_map>

#include "injector.h"
#include "sock_bridge.h"

const u_int16_t BLOCK_SIZE = 1024;

int DEF_CB(SockBridge &obj) {
    u_int8_t arr[BLOCK_SIZE];
    int br = obj.read(arr, sizeof(arr));
    if (br > 0)
        obj.write(arr, br);

    return br;
}

inline void Injector::set_non_block(int fd) {
    int flags, s;

    flags = fcntl (fd, F_GETFL, 0);
    if (flags == -1) {
        throw SocketError("fcntl F_GETFL error", errno);
    }

    flags |= O_NONBLOCK;
    s = fcntl (fd, F_SETFL, flags);
    if (s == -1) {
        throw SocketError("fcntl F_SETFL error", errno);
    }
}

inline int Injector::create_and_bind_s(u_int16_t port) {
    struct sockaddr_in addr;

    int fd = socket (AF_INET, SOCK_STREAM, 0);

    if (master_socket == -1) {
        throw SocketError("socket error", errno);
    }

    int enable = 1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        throw SocketError("setsockopt error", errno);
    }

    enable = 1;

    if (setsockopt(fd, SOL_IP, IP_TRANSPARENT, &enable, sizeof(enable)) < 0) {
        throw SocketError("setsockopt error", errno);
    }

    if (bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
        throw SocketError("bind error", errno);
    }

    if (listen(fd, SOMAXCONN) < 0) {
        throw SocketError("listen error", errno);
    }

    return fd;
}


Injector::Injector(u_int16_t port) {
    struct epoll_event event;

    try {
        this->master_socket = create_and_bind_s(port);
        this->efd = epoll_create1(0);
        if (this->efd == -1) {
            throw SocketError("epoll_create1 error", errno);
        }

        this->set_non_block(this->master_socket);

        event.data.fd = this->master_socket;
        event.events = EPOLLIN;
        if (epoll_ctl (this->efd, EPOLL_CTL_ADD, this->master_socket, &event) < 0) {
            throw SocketError("epoll_ctl ADD error", errno);
        }

    } catch(...) {
        throw;
    }

}

inline int Injector::connect_to_dst_host(struct sockaddr_in &addr) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd == -1) {
        throw SocketError("socket error", errno);
    }

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        throw SocketError("connect error", errno);
    }

    return sock_fd;
}


inline void Injector::add_fd_to_map(int local_fd, int dst_fd, bool state) {
    socks_info *arg = new socks_info;
    arg->fd = dst_fd;
    arg->status = state;
    this->s_fds[local_fd] = arg;
}

inline void Injector::add_fds_to_map(int local_fd, int dst_fd) {
    try {
        this->add_fd_to_map (local_fd, dst_fd, OUT);
    }
    catch (std::bad_alloc &err) {
        throw;
    }

    try {
        this->add_fd_to_map(dst_fd, local_fd, IN);
    }
    catch (std::bad_alloc &err) {
        this->remove_fd_from_map (local_fd);
        throw;
    }
}



inline void Injector::remove_fd_from_map(int fd) {
    close(fd);
    delete this->s_fds.find(fd)->second;
    s_fds.erase(fd);
}

inline void Injector::add_fd_to_epoll (int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl (this->efd, EPOLL_CTL_ADD, fd, &event)) {
        throw SocketError("epoll error", errno);
    }
}

inline void Injector::close_connections(int local_fd) {
    socks_info *val_ptr = this->s_fds.find(local_fd)->second;
    this->remove_fd_from_map(val_ptr->fd);
    this->remove_fd_from_map(local_fd);
}


inline void Injector::handle_in(int local_fd, injector_cb in_cb, injector_cb out_cb) {
    socks_info *val_ptr = this->s_fds.find(local_fd)->second;
    SockBridge sb_arg(local_fd, val_ptr->fd);
    ssize_t br;

    try {
        br = (val_ptr->status == OUT ? out_cb(sb_arg) : in_cb(sb_arg));
    }
    catch (SockBridgeException &err) {
        #ifdef DEBUG
            std::cerr << err.get_msg() << std::endl;
        #endif
        br = 0;
    }

    if (br == 0) {
        close_connections(local_fd);
        #ifdef DEBUG
            std::cout << "Delete socket with fd = " <<  local_fd << std::endl;
        #endif
    }
}


void Injector::loop (injector_cb in_cb, injector_cb out_cb) {

    epoll_event *events = new epoll_event[SOMAXCONN];

    while (1) {
        int n = epoll_wait (this->efd, events, SOMAXCONN, -1);
        for (size_t i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
               (events[i].events & EPOLLHUP) ||
               (!(events[i].events & EPOLLIN))) {

                continue;
            }
            else if(this->master_socket == events[i].data.fd) {
                int local_fd = accept(this->master_socket, 0, 0);
                if (local_fd == -1)
                    throw SocketError("accept error", errno);

                #ifdef DEBUG
                    std::cout << "Connect socket with fd = " <<  local_fd << std::endl;
                #endif

                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                if (getsockname(local_fd, (struct sockaddr*)&addr, &len) == -1) {
                    continue;
                }

                int dst_fd = 0;

                try {
                    dst_fd = this->connect_to_dst_host(addr);
                } catch (SocketError &err) {
                    #ifdef DEBUG
                        std::cerr << err.get_msg() << std::endl;
                    #endif
                    close(local_fd);
                    continue;
                }

                this->set_non_block(dst_fd);

                try {
                    this->add_fds_to_map(local_fd, dst_fd);
                }
                catch (std::bad_alloc &err) {
                    continue;
                }

                try {
                    this->add_fd_to_epoll(local_fd);
                    this->add_fd_to_epoll(dst_fd);
                }
                catch (SocketError &err) {
                    throw;
                }
            }
            else {
                handle_in(events[i].data.fd, in_cb, out_cb);
            }
        }
    }
}
