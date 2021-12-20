#ifndef NETLIB_SSL_TCP_CLIENT_CONTEXT_HPP
#define NETLIB_SSL_TCP_CLIENT_CONTEXT_HPP


#include "ssl_context.hpp"


namespace netlib::ssl::tcp {


    /**
     * Client context for tcp ssl sockets.
     */
    class client_context : public ssl::context {
    public:
        /**
         * Constructor.
         * @param certificate_file certificate file (normally has 'pem' extension).
         * @param key_file key file (normally has 'key' extension).
         * @exception ssl_error thrown if there was an error.
         */
        client_context(const char* certificate_file, const char* key_file);
    };


} //namespace netlib::ssl::tcp


#endif //NETLIB_SSL_TCP_CLIENT_CONTEXT_HPP
