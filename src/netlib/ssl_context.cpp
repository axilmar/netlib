#include "ssl.hpp"
#include "netlib/ssl_context.hpp"


namespace netlib::ssl {


    //The constructor.
    context::context(const std::shared_ptr<ssl_ctx_st>& ctx) : m_ctx(ctx) {
        //in debug mode, accept all ciphers
        #ifndef NDEBUG
        SSL_CTX_set_cipher_list(ctx.get(), "ALL:NULL:eNULL:aNULL");
        #endif

        //other useful options
        //SSL_CTX_set_session_cache_mode(ctx.get(), SSL_SESS_CACHE_OFF);
    }


} //namespace netlib::ssl
