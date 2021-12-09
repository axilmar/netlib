#include "platform.hpp"
#include "netlib/socket.hpp"


namespace netlib {


    //close socket.
    static void socket_close(uintptr_t handle) {
        if (handle != ~uintptr_t(0)) {
            closesocket(handle);
        }
    }


    //closes the socket.
    socket::~socket() {
        socket_close(m_handle);
    }


    //if socket is initialized
    socket::operator bool() const {
        return m_handle != ~uintptr_t(0);
    }


    //the default constructor.
    socket::socket(uintptr_t handle) : m_handle(handle) {}


    //The move constructor.
    socket::socket(socket&& src) : m_handle(src.m_handle) {
        src.m_handle = ~uintptr_t(0);
    }


    //The move assignment operator.
    socket& socket::operator = (socket&& src) {
        if (&src != this) {
            socket_close(m_handle);
            m_handle = src.m_handle;
            src.m_handle = ~uintptr_t(0);
        }
        return *this;
    }


} //namespace netlib
