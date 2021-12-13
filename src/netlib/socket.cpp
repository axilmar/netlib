#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/socket.hpp"


namespace netlib {


    //the default constructor.
    socket::socket(handle_type handle)
        : m_handle(handle)
    {
    }


    //closes the socket.
    socket::~socket() {
        if (m_handle != invalid_handle) {
            closesocket(m_handle);
        }
    }


    //The move constructor.
    socket::socket(socket&& src)
        : m_handle{ src.m_handle }
    {
        src.m_handle = invalid_handle;
    }


    //The move assignment operator.
    socket& socket::operator = (socket&& src) {
        handle_type temp = src.m_handle;
        src.m_handle = invalid_handle;
        m_handle = temp;
        return *this;
    }


    //Returns the handle.
    socket::handle_type socket::handle() const { 
        return m_handle;
    }


    //if socket is initialized
    socket::operator bool() const {
        return m_handle != invalid_handle;
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


    //compare sockets.
    int socket::compare(const socket& other) const {
        return m_handle < other.m_handle ? -1 : m_handle > other.m_handle ? 1 : 0;
    }


    /**
     * Returns the hash code for this object.
     */
    size_t socket::hash() const {
        return std::hash<handle_type>()(m_handle);
    }


} //namespace netlib
