#include "platform.hpp"
#include <system_error>
#include "ssl.hpp"
#include "openssl/err.h"
#include "netlib/ssl_error.hpp"


namespace netlib::ssl {


    //handle io error
    ssl_io_result ssl_handle_io_error(SSL* ssl, int bytes) {
        int error = SSL_get_error(ssl, bytes);

        //if error is SSL_ERROR_NONE, it means zero bytes were successfully sent
        if (error == SSL_ERROR_NONE) {
            return ssl_io_result::success;
        }

        //if error is SSL_ERROR_ZERO_RETURN, it means 'connection closed'
        if (error == SSL_ERROR_ZERO_RETURN) {
            return ssl_io_result::failure;
        }

        //for these values, throw custom error
        switch (error) {
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_CONNECT:
        case SSL_ERROR_WANT_ACCEPT:
        case SSL_ERROR_WANT_ASYNC:
        case SSL_ERROR_WANT_ASYNC_JOB:
            throw std::logic_error("Non-blocking SSL I/O unsupported.");
        }

        //for these values, repeat
        switch (error) {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_ACCEPT:
        case SSL_ERROR_WANT_X509_LOOKUP:
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            return ssl_io_result::retry;
        }

        //throw system error
        if (error == SSL_ERROR_SYSCALL) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }

        //an ssl error
        if (error == SSL_ERROR_SSL) {
            throw ssl::error(ERR_get_error());
        }

        //other error
        throw ssl::error("Unsupported error code: " + std::to_string(error));
    }


} //namespace netlib::ssl
