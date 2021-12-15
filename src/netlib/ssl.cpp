#include "platform.hpp"
#include <system_error>
#include <thread>
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
        case SSL_ERROR_WANT_X509_LOOKUP:
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            return ssl_io_result::retry;
        }

        //throw system error
        if (error == SSL_ERROR_SYSCALL) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //an ssl error
        if (error == SSL_ERROR_SSL) {
            throw ssl::error(ERR_get_error());
        }

        //other error
        throw ssl::error("Unsupported error code: " + std::to_string(error));
    }


    //SSL destructor.
    static void SSL_destructor(SSL* ssl) {
        int sock = SSL_get_fd(ssl);
        SSL_free(ssl);
        closesocket(sock);
    }


    //does shutdown/SSL_free/close socket.
    void SSL_close(SSL* ssl) {
        const int r = SSL_shutdown(ssl);

        //normal shutdown
        if (r == 1) {
            SSL_destructor(ssl);
            return;
        }

        //do SSL read until return value <= 0
        if (r == 0) {
            char buf[64];
            for (bool loop = true; loop;) {
                const int s = SSL_read(ssl, buf, sizeof(buf));

                //continue reading
                if (s > 0) {
                    continue;
                }

                //handle error
                switch (SSL_get_error(ssl, s)) {
                //no error invalid at this stage
                case SSL_ERROR_NONE:
                    SSL_destructor(ssl);
                    throw ssl::error("No error returned by SSL_get_error althrough SSL_read returned value <= 0.");

                //peer closed the connection
                case SSL_ERROR_ZERO_RETURN:
                    SSL_destructor(ssl);
                    return;

                //no support for async
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                case SSL_ERROR_WANT_CONNECT:
                case SSL_ERROR_WANT_ACCEPT:
                case SSL_ERROR_WANT_ASYNC:
                case SSL_ERROR_WANT_ASYNC_JOB:
                    throw std::logic_error("Non-blocking SSL I/O unsupported.");

                //retry
                case SSL_ERROR_WANT_X509_LOOKUP:
                case SSL_ERROR_WANT_CLIENT_HELLO_CB:
                    continue;

                //system error
                case SSL_ERROR_SYSCALL:
                    SSL_destructor(ssl);
                    throw std::system_error(get_last_error_number(), std::system_category());

                //ssl error
                default:
                    SSL_destructor(ssl);
                    throw ssl::error(ERR_get_error());
                }
            }
        }

        //get error
        int error = SSL_get_error(ssl, r);

        //the ssl is no longer needed
        SSL_destructor(ssl);

        //this should not happen at this moment
        if (error == SSL_ERROR_NONE) {
            throw ssl::error("No error returned by SSL_get_error althrough SSL_shutdown returned value < 0.");
        }

        //this should not happen at this moment
        if (error == SSL_ERROR_ZERO_RETURN) {
            throw ssl::error("Zero return error returned by SSL_get_error althrough SSL_shutdown returned value < 0.");
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

        //for these values, throw ssl error
        switch (error) {
        case SSL_ERROR_WANT_X509_LOOKUP:
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            throw ssl::error("Error code indicating retry returned by SSL_get_error althrough SSL_shutdown returned value < 0.");
        }

        //throw system error
        if (error == SSL_ERROR_SYSCALL) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //throw ssl error
        if (error == SSL_ERROR_SSL) {
            throw ssl::error(ERR_get_error());
        }

        //other error
        throw ssl::error("Unsupported error code: " + std::to_string(error));
    }


} //namespace netlib::ssl
