#ifndef NETLIB_SSL_SERVER_CONTEXT_HPP
#define NETLIB_SSL_SERVER_CONTEXT_HPP


#include "ssl_context.hpp"


namespace netlib::ssl {


    /**
     * Base class for server contexts.
     */
    class server_context : public context {
    public:

    protected:
        /**
         * Constructor.
         * @param ctx context.
         * @param certificate_file certificate file.
         * @param key_file key file.
         * @exception ssl_error thrown if there was an error.
         */
        server_context(const std::shared_ptr<ssl_ctx_st>& ctx, const char* certificate_file, const char* key_file);
    };


} //namespace netlib::ssl


#endif //NETLIB_SSL_SERVER_CONTEXT_HPP
