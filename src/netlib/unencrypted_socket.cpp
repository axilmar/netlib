#include "platform.hpp"
#include "netlib/unencrypted_socket.hpp"


namespace netlib::unencrypted {


    //Closes the underlying socket.
    socket::~socket() {
        closesocket(m_handle);
    }


} //namespace netlib::unencrypted
