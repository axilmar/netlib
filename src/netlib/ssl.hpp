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


    //does shutdown/SSL_free/close socket.
    void SSL_close(SSL* ssl);


    //send data
    bool ssl_send(SSL* ssl, const char* d, int len);


    //receive data
    bool ssl_receive(SSL* ssl, char* d, int len);


    } //namespace netlib::ssl


#endif //NETLIB_SSL_HPP
