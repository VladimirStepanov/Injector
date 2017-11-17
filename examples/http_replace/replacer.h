#include <unordered_map>
#include <vector>
#include <string>
#include "sock_bridge.h"


#ifndef REPLACER_H
#define REPLACER_H

using std::vector;
using std::string;


class Replacer {
private:
    std::unordered_map<int, vector<char>*> socks;
    char* get_http_data(char *data, size_t data_len);

public:
    void save(int fd, char *arr, size_t size);
    void send_all(SockBridge &sb_obj, size_t pack_size);
    size_t count_words(const vector<char> &v, const string &word);
    void fix_http_cl(vector<char> &v, int diff);
    void replace(vector<char> &v, const string &o_word, const string &n_word);
    void replace_all(vector<char> &str, const string &o_word, const string &n_word);
    void http_replace(int fd, const string &o_word, const string &n_word);
    bool check_content_type(const vector<char> &v, const char *type);
    void push_chunk_to_v(vector<char> &v, char *arr, size_t size);
};


#endif