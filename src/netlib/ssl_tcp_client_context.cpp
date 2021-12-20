#include "ssl.hpp"
#include "netlib/ssl_tcp_client_context.hpp"


namespace netlib::ssl::tcp {


    //create context
    static std::shared_ptr<SSL_CTX> create_context() {
        const SSL_METHOD* method = TLS_client_method();
        return { SSL_CTX_new(method), SSL_CTX_free };
    }


    //constructor
    client_context::client_context(const char* certificate_file, const char* key_file)
        : ssl::context(create_context(), certificate_file, key_file)
    {
    }


} //namespace netlib::ssl::tcp
