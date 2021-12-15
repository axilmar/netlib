#include <string>
#include "openssl/err.h"
#include "netlib/ssl_error.hpp"


namespace netlib::ssl {


    //get the error string from openssl
    static std::string get_openssl_error_string(unsigned long code) {
        char buf[4096];
        ERR_error_string_n(code, buf, sizeof(buf));
        return buf;
    }


    //constructor
    error::error(unsigned long code) 
        : std::runtime_error(get_openssl_error_string(code))
        , m_code(code)
    {
    }


    //Constructor from string.
    error::error(const std::string& str)
        : std::runtime_error(str)
        , m_code(0)
    {
    }


} //namespace netlib::ssl
