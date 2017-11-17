#include <iostream>

#include "injector.h"
#include "sock_bridge.h"


int main() {
    try {
        Injector obj;
        obj.loop(DEF_CB);
    } catch (SocketError &err) {
        std::cout << err.get_msg();
    }
    return 0;
}