#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/socket.hpp"


namespace netlib {


    //if socket is initialized
    socket::operator bool() const {
        return handle() != invalid_handle;
    }


    //Returns the address this socket is connected to.
    socket_address socket::peer_address(handle_type socket) {
        sockaddr_storage s;
        int namelen = sizeof(s);

        if (getpeername(socket, reinterpret_cast<sockaddr*>(&s), &namelen)) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        switch (s.ss_family) {
        case AF_INET:
            return socket_address(ntohl(reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(&s)->sin_addr)), ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));

        case AF_INET6:
            return socket_address({ reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(&s)->sin6_addr), reinterpret_cast<const sockaddr_in6*>(&s)->sin6_scope_id }, ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns the address this socket is bound to.
    socket_address socket::bound_address(handle_type socket) {
        sockaddr_storage s;
        int namelen = sizeof(s);

        if (getsockname(socket, reinterpret_cast<sockaddr*>(&s), &namelen)) {
            throw std::system_error(get_last_error_number(), std::system_category());
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
        return handle() < other.handle() ? -1 : handle() > other.handle() ? 1 : 0;
    }


    /**
     * Returns the hash code for this object.
     */
    size_t socket::hash() const {
        return std::hash<handle_type>()(handle());
    }


    //Sets SO_REUSEADDR and SO_REUSEPORT (if available) on the underlying socket handle.
    void socket::set_reuse_address_and_port(handle_type handle) {
        const char on = 1;

        if (setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        #ifdef SO_REUSEPORT
        if (setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
        #endif
    }


} //namespace netlib
