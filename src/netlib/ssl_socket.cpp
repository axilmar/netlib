#include "openssl/ssl.h"
#include "netlib/ssl_socket.hpp"


namespace netlib::ssl {


    //Returns the handle.
    netlib::socket::handle_type socket::handle() const {
        return m_ssl ? SSL_get_fd(m_ssl.get()) : invalid_handle;
    }


} //namespace netlib::ssl
