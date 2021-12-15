#include "ssl.hpp"
#include "netlib/ssl_tcp_server_context.hpp"


namespace netlib::ssl::tcp {


    //create context
    static std::shared_ptr<SSL_CTX> create_context() {
        const SSL_METHOD* method = TLS_server_method();
        return {SSL_CTX_new(method), SSL_CTX_free};
    }


    //constructor
    server_context::server_context(const char* certificate_file, const char* key_file)
        : ssl::server_context(create_context(), certificate_file, key_file)
    {
    }


} //namespace netlib::ssl::tcp
