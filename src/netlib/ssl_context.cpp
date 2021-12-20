#include "ssl.hpp"
#include "netlib/ssl_context.hpp"
#include "netlib/ssl_error.hpp"


namespace netlib::ssl {


    //The constructor.
    context::context(const std::shared_ptr<ssl_ctx_st>& ctx, const char* certificate_file, const char* key_file) 
    : m_ctx(ctx)
    {
        //load the certificate file.
        if (SSL_CTX_use_certificate_file(ctx.get(), certificate_file, SSL_FILETYPE_PEM) <= 0)
        {
            throw error(ERR_get_error());
        }

        //load the private key file
        if (SSL_CTX_use_PrivateKey_file(ctx.get(), key_file, SSL_FILETYPE_PEM) <= 0)
        {
            throw error(ERR_get_error());
        }

        //verify the private key
        if (!SSL_CTX_check_private_key(ctx.get()))
        {
            throw error("Invalid private key");
        }
    }


} //namespace netlib::ssl
