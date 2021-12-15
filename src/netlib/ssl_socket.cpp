#include "platform.hpp"
#include "openssl/ssl.h"
#include "netlib/ssl_socket.hpp"


namespace netlib::ssl {


    //Closes the socket.
    socket::~socket() {
        //closesocket(handle());
    }


    //Returns the handle.
    netlib::socket::handle_type socket::handle() const {
        return m_ssl ? SSL_get_fd(m_ssl.get()) : invalid_handle;
    }


} //namespace netlib::ssl
