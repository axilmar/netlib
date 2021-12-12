#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/socket.hpp"


namespace netlib {


    //close socket.
    static void close_socket(uintptr_t handle) {
        if (handle != ~uintptr_t(0)) {
            closesocket(handle);
        }
    }


    //closes the socket.
    socket::~socket() {
        close_socket(m_handle);
    }


    //if socket is initialized
    socket::operator bool() const {
        return m_handle != ~uintptr_t(0);
    }


    //Returns the address this socket is bound to.
    socket_address socket::bound_address() const {
        sockaddr_storage s;
        int namelen = sizeof(s);

        if (getsockname(m_handle, reinterpret_cast<sockaddr*>(&s), &namelen)) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }

        switch (s.ss_family) {
        case AF_INET:
            return socket_address(ntohl(reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(&s)->sin_addr)), ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));

        case AF_INET6:
            return socket_address({ reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(&s)->sin6_addr), reinterpret_cast<const sockaddr_in6*>(&s)->sin6_scope_id }, ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));
        }

        throw std::logic_error("Invalid address family.");
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
            close_socket(m_handle);
            m_handle = src.m_handle;
            src.m_handle = ~uintptr_t(0);
        }
        return *this;
    }


} //namespace netlib
