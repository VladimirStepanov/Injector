#include <cxxtest/TestSuite.h>
#include <vector>
#include <string>
#include <string.h>
#include "replacer.h"

using std::string;
using std::vector;

class ReplacerTest : public CxxTest::TestSuite {
private:
    Replacer obj;
    vector<char> vec_str;
    vector<char> cont_vt;
    string o_word = "hello";
    const char* test_content_type = "blabla Content-Type: text/html bla bla";
    const char* test_string = "\r\n\r\nhello worldhellw hello test hello";
    const char* con_length_gr = "Content-Length: 999 ";
    const char* con_length_eq = "Content-Length: 555 ";
    const char* con_length_less = "Content-Length: 100 ";

    void fill_vector(vector <char> &v, const char *str) {
        for (size_t i = 0; i < strlen(str) + 1; i++)
            v.push_back(str[i]);
    }
    
public:

    void setUp() {
        vec_str.clear();
        fill_vector(this->vec_str, this->test_string);

        cont_vt.clear();
    }

    void test_count() {
        string res = "hello";
        TS_ASSERT_EQUALS(obj.count_words(vec_str, res), 3);
    }

    void test_bad_count() {
        string res = "\r\n\r\nhello worldhellw hello test hellowwww";
        TS_ASSERT_EQUALS(obj.count_words(vec_str, res), 0);
    }

    void test_eq_replace() {
        string n_word = "world";
        string res = "\r\n\r\nworld worldhellw world test world";
        obj.replace(vec_str, o_word, n_word);
        TS_ASSERT_EQUALS(strncmp(vec_str.data(), res.c_str(), vec_str.size()), 0);
    }

    void test_less_replace() {
        string n_word = "wo";
        string res = "\r\n\r\nwo worldhellw wo test wo";
        obj.replace(vec_str, o_word, n_word);
        TS_ASSERT_EQUALS(strncmp(vec_str.data(), res.c_str(), vec_str.size()), 0);
    }

    void test_gr_replace() {
        string n_word = "wwwwww";
        string res = "\r\n\r\nwwwwww worldhellw wwwwww test wwwwww";
        obj.replace(vec_str, o_word, n_word);
        TS_ASSERT_EQUALS(strncmp(vec_str.data(), res.c_str(), vec_str.size()), 0);
    }

    void test_fix_cl_eq() {
        const char* con_length_eq_res = "Content-Length: 666 ";
        fill_vector(this->cont_vt, con_length_eq);
        obj.fix_http_cl(cont_vt, 111);
        TS_ASSERT_EQUALS(strncmp(this->cont_vt.data(), con_length_eq_res, this->cont_vt.size()), 0);
    }

    void test_fix_cl_less() {
        const char* con_length_eq_less = "Content-Length: 98 ";
        fill_vector(this->cont_vt, con_length_less);
        obj.fix_http_cl(this->cont_vt, -2);
        TS_ASSERT_EQUALS(strncmp(this->cont_vt.data(), con_length_eq_less, this->cont_vt.size()), 0);
    }

    void test_fix_cl_gr() {
        const char* con_length_eq_gr = "Content-Length: 1001 ";
        fill_vector(this->cont_vt, con_length_gr);
        obj.fix_http_cl(this->cont_vt, 2);
        TS_ASSERT_EQUALS(strncmp(this->cont_vt.data(), con_length_eq_gr, this->cont_vt.size()), 0);
    }

    void test_good_content_type() {
        fill_vector(this->cont_vt, test_content_type);
        bool res = obj.check_content_type(this->cont_vt, "text/html");

        TS_ASSERT_EQUALS(res, true);
    }

    void test_bad_content_type() {
        fill_vector(this->cont_vt, test_content_type);
        bool res = obj.check_content_type(this->cont_vt, "hello/world");

        TS_ASSERT_EQUALS(res, false);
    }


    void test_push_chunk() {
        char test_arr[3] = {1, 2, 3};
        obj.push_chunk_to_v(cont_vt, test_arr, sizeof(test_arr));
        obj.push_chunk_to_v(cont_vt, test_arr, sizeof(test_arr));
        
        TS_ASSERT_EQUALS(cont_vt.size(), sizeof(test_arr) * 2);

        for (size_t i = 0, j = 3; i < sizeof(test_arr); i++, j++) {
            TS_ASSERT_EQUALS(cont_vt[i], test_arr[i]);
            TS_ASSERT_EQUALS(cont_vt[j], test_arr[i]);
        }
    }

};