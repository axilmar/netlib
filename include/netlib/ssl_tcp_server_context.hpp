#ifndef NETLIB_SSL_TCP_SERVER_CONTEXT_HPP
#define NETLIB_SSL_TCP_SERVER_CONTEXT_HPP


#include "ssl_server_context.hpp"


namespace netlib::ssl::tcp {


    /**
     * Server context for tcp ssl sockets.
     */
    class server_context : public ssl::server_context {
    public:
        /**
         * Constructor.
         * @param certificate_file certificate file (normally has 'pem' extension).
         * @param key_file key file (normally has 'key' extension).
         * @exception ssl_error thrown if there was an error.
         */
        server_context(const char* certificate_file, const char* key_file);
    };


} //namespace netlib::ssl::tcp


#endif //NETLIB_SSL_TCP_SERVER_CONTEXT_HPP
