#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <sstream>



class ExceptionBase {
private:
    std::string res_msg;

public:
    ExceptionBase(std::string m, int ec)  {
        std::ostringstream oss;
        oss << m << " " << ec << std::endl;
        res_msg += oss.str();
    } 

    std::string get_msg() const {
        return res_msg;
    }
};


#endif