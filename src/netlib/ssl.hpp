#ifndef NETLIB_SSL_HPP
#define NETLIB_SSL_HPP


#include "openssl/ssl.h"
#include "openssl/err.h"


namespace netlib::ssl {


    //io result
    enum class ssl_io_result {
        failure,
        success,
        retry
    };


    //handle io error
    ssl_io_result ssl_handle_io_error(SSL* ssl, int bytes);


} //namespace netlib::ssl


#endif //NETLIB_SSL_HPP
