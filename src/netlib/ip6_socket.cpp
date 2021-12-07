#include "platform.hpp"
#include "netlib/ip6_socket.hpp"


namespace netlib::ip6 {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //Returns the assigned address of the socket.
    socket_address socket::get_assigned_address() const {
        sockaddr_in6 a;
        int namelen = sizeof(a);
        if (getsockname(handle(), reinterpret_cast<sockaddr*>(&a), &namelen)) {
            throw std::runtime_error(get_last_error_message());
        }
        return { {reinterpret_cast<const address::bytes_type&>(a.sin6_addr), a.sin6_scope_id}, ntohs(a.sin6_port) };
    }


    ///////////////////////////////////////////////////////////////////////////
    // PROTECTED
    ///////////////////////////////////////////////////////////////////////////


    //Binds the socket at the specified address.
    void socket::bind(const socket_address& addr) {
        sockaddr_in6 a{};
        a.sin6_addr = reinterpret_cast<const in6_addr&>(addr.address().bytes());
        a.sin6_family = AF_INET6;
        a.sin6_port = htons(addr.port_number());
        a.sin6_scope_id = addr.address().zone_index();

        int error = ::bind(handle(), reinterpret_cast<sockaddr*>(&a), sizeof(a));

        if (error) {
            throw std::runtime_error(get_last_error_message());
        }
    }


    //Connects the socket at the specified address.
    void socket::connect(const socket_address& addr) {
        sockaddr_in6 a{};
        a.sin6_addr = reinterpret_cast<const in6_addr&>(addr.address().bytes());
        a.sin6_family = AF_INET6;
        a.sin6_port = htons(addr.port_number());
        a.sin6_scope_id = addr.address().zone_index();

        int error = ::connect(handle(), reinterpret_cast<sockaddr*>(&a), sizeof(a));

        if (error) {
            throw std::runtime_error(get_last_error_message());
        }
    }


} //namespace netlib::ip6
