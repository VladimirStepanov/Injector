#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <string>

#include "injector.h"
#include "sock_bridge.h"
#include "replacer.h"

const size_t ARR_SIZE = 4096;
std::string old_word, new_word;

// Wait when sock will be close
// not work for keep-alive
int http_replace(SockBridge &sb_obj) {
	static char arr[ARR_SIZE];
	static Replacer robj;
	ssize_t br = sb_obj.read((uint8_t*)arr, ARR_SIZE);

	if (br > 0) {
		robj.save(sb_obj.get_from_fd(), arr, br);
	}
	else {
		robj.http_replace(sb_obj.get_from_fd(), old_word, new_word);
		robj.send_all(sb_obj, ARR_SIZE);
	}


	return br;
}



int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "Usage:" << argv[0] << " old_word new_word" << std::endl;
		exit(0);
	}

	old_word = argv[1];
	new_word = argv[2];

    try {
        Injector obj;
        obj.loop(http_replace);
    } catch (SocketError &err) {
        std::cout << err.get_msg();
    }
    return 0;
}