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
         * @exception ssl_error thrown if there was an error.
         */
        client_context();
    };


} //namespace netlib::ssl::tcp


#endif //NETLIB_SSL_TCP_CLIENT_CONTEXT_HPP
