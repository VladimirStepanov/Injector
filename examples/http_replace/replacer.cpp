#include <string.h>
#include <vector>
#include <string>
#include "replacer.h"
#include "sock_bridge.h"


const char* CONTENT_TYPE = "text/html";

//get start pointer to http data
char* Replacer::get_http_data(char *data, size_t data_len) {
    const string empty = "\r\n\r\n";

    for (size_t i = 0; i < data_len - empty.size(); i++) {
        if (!memcmp(data + i, empty.c_str(), empty.size()))
            return data + i + empty.size();
    }
    return NULL;
}


//add array to vector via memcpy
void Replacer::push_chunk_to_v(vector<char> &v, char *arr, size_t size) {
    size_t old_size = v.size();
    v.resize(size + old_size);
    memcpy(v.data() + old_size, arr, size);
}

//compare content-type with @type parameter
bool Replacer::check_content_type(const vector<char> &v, const char *type) {
    std::string content_type = string("Content-type: ") + type;
    std::string ocontent_type = string("Content-Type: ") + type;

    return (strstr(v.data(), content_type.c_str()) || strstr(v.data(), ocontent_type.c_str())) ? true : false;
}


//write data to vector
void Replacer::save(int fd, char *arr, size_t size) {
    std::vector<char>* v_ptr;
    auto it = this->socks.find(fd);
    if (it != socks.end()) {
        v_ptr = it->second;
    }
    else {
        v_ptr = new std::vector<char>;
        this->socks[fd] = v_ptr;
    }

    push_chunk_to_v(*v_ptr, arr, size);
}


//send all data from vector to sb_obj.to 
void Replacer::send_all(SockBridge &sb_obj, size_t pack_size) {
    auto it = this->socks.find(sb_obj.get_from_fd());
    std::vector<char>* v_ptr = it->second;
    size_t cur_size = 0;

    for (; (cur_size + pack_size) < v_ptr->size(); cur_size += pack_size) {
        sb_obj.write((uint8_t*)v_ptr->data() + cur_size, pack_size);
    }

    if (cur_size < v_ptr->size()) {
        sb_obj.write((uint8_t*)v_ptr->data() + cur_size, v_ptr->size() - cur_size);
    }

    delete v_ptr;
    this->socks.erase(sb_obj.get_from_fd());

}


//count words @word in data
size_t Replacer::count_words(const vector<char> &v, const string &word) {
    size_t count = 0;

    if (word.size() < v.size()) {
        for (const char *start = strstr(v.data(), word.c_str()); 
             start; 
             start += word.size(), start  = strstr(start, word.c_str()), count++) {}
    }

    return count;
}

//edit http content-length and edit this into header: old_val + diff
void Replacer::fix_http_cl(vector<char> &v, int diff) {
    const char* con_length_tmpl = "Content-Length: ";
    string cl_string = "";
    char *res = strstr(v.data(), con_length_tmpl);
    if (res) {
        char *nmb_ptr = res + strlen(con_length_tmpl);
        for (size_t i = 0; isdigit(nmb_ptr[i]); i++) {
            cl_string += nmb_ptr[i];
        }

        string n_cl_string = std::to_string(std::stoi(cl_string) + diff);

        int cld = n_cl_string.size() - cl_string.size();//content-length diff

        if (cld != 0) {
            if (cld > 0) // if new content-length greater then old, resize header to difference between cl's and put instead old
                v.resize(v.size() + cld);
            memmove(nmb_ptr + n_cl_string.size(), nmb_ptr + cl_string.size(), strlen(nmb_ptr + n_cl_string.size()) + 1);
            memcpy(nmb_ptr, n_cl_string.c_str(), n_cl_string.size());
            if (cld < 0)//if new content-length less than old, at first put new cl into header, then resize back vector and put instead old
                v.resize(v.size() + cld);
        }
        else {
            memcpy(nmb_ptr, n_cl_string.c_str(), n_cl_string.size());
        }
    }
}


//replace all words in text o_word -> n_word
void Replacer::replace_all(vector<char> &v, const string &o_word, const string &n_word) {

    char *res = get_http_data(v.data(), v.size());
    if (!res)
        return;

    while ((res = strstr(res, o_word.c_str()))) {
        int diff = n_word.size() - o_word.size();
        if (diff != 0) {
            memmove (res + n_word.size(), res + o_word.size(), strlen(res + o_word.size()) + 1);
        } 
        memcpy (res, n_word.c_str(), n_word.size());
        res = res + n_word.size();
    }

}


void Replacer::replace(vector<char> &v, const string &o_word, const string &n_word) {
    size_t cw = this->count_words(v, o_word);
    if (cw > 0) {
        int diff = n_word.size() - o_word.size();
        this->fix_http_cl(v, diff);
        if (o_word.size() < n_word.size()) {
            v.resize(v.size() + diff * cw);
            replace_all(v, o_word, n_word);
        }
        else if (o_word.size() > n_word.size()) {
            replace_all(v, o_word, n_word);
            v.resize(v.size() + diff*cw);
        }
        else {
            replace_all(v, o_word, n_word);
        }
    }
}

//interface for main
void Replacer::http_replace(int fd, const string &o_word, const string &n_word) {
    auto it = this->socks.find(fd);
    if (it != this->socks.end()) {
        std::vector<char>* v_ptr = it->second;
        if (check_content_type(*v_ptr, CONTENT_TYPE)) {
            replace(*v_ptr, o_word, n_word);
        }
    }
}
