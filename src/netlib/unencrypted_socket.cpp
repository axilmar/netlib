#include "platform.hpp"
#include "netlib/unencrypted_socket.hpp"


namespace netlib::unencrypted {


    //Closes the underlying socket.
    socket::~socket() {
        closesocket(m_handle);
    }


    //The move assignment operator.
    socket& socket::operator = (socket&& src) {
        handle_type temp = src.m_handle;
        src.m_handle = invalid_handle;
        m_handle = temp;
        return *this;
    }


} //namespace netlib::unencrypted
