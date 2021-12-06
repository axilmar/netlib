#include "platform.hpp"
#include "numeric_cast.hpp"
#include "netlib/socket.hpp"


namespace netlib {


    ///////////////////////////////////////////////////////////////////////////
    // INTERNALS
    ///////////////////////////////////////////////////////////////////////////


    //Invalid socket handle.
    static constexpr socket_handle invalid_socket_handle = ~socket_handle(0);


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //Closes the socket handle.
    socket::~socket() {
        close();
    }


    //check validity.
    socket::operator bool() const {
        return m_handle != invalid_socket_handle;
    }


    ///////////////////////////////////////////////////////////////////////////
    // PROTECTED
    ///////////////////////////////////////////////////////////////////////////


    //the invalid constructor.
    socket::socket() : m_handle(invalid_socket_handle) {
    }


    //The constructor from handle.
    socket::socket(socket_handle handle) : m_handle(handle) {
    }


    //The move constructor.
    socket::socket(socket&& src) : m_handle(src.m_handle) {
        src.m_handle = invalid_socket_handle;
    }


    //The move assignment operator.
    socket& socket::operator = (socket&& src) {
        if (&src != this) {
            close();
            m_handle = src.m_handle;
            src.m_handle = invalid_socket_handle;
        }
        return *this;
    }


    ///////////////////////////////////////////////////////////////////////////
    // PRIVATE
    ///////////////////////////////////////////////////////////////////////////


    //closes the socket.
    void socket::close() {
        if (m_handle != invalid_socket_handle) {
            closesocket(m_handle);
        }
    }


} //namespace netlib
