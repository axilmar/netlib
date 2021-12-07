#include "platform.hpp"
#include "netlib/ip4_socket.hpp"


namespace netlib::ip4 {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //Returns the assigned address of the socket.
    socket_address socket::get_assigned_address() const {
        sockaddr_in a;
        int namelen = sizeof(a);
        if (getsockname(handle(), reinterpret_cast<sockaddr*>(&a), &namelen)) {
            throw std::runtime_error(get_last_error_message());
        }
        return { {reinterpret_cast<const address::bytes_type&>(a.sin_addr)}, ntohs(a.sin_port) };
    }


    ///////////////////////////////////////////////////////////////////////////
    // PROTECTED
    ///////////////////////////////////////////////////////////////////////////


    //Binds the socket at the specified address.
    void socket::bind(const socket_address& addr) {
        sockaddr_in a{};
        a.sin_addr.S_un.S_addr = addr.address().value();
        a.sin_family = AF_INET;
        a.sin_port = htons(addr.port_number());

        int error = ::bind(handle(), reinterpret_cast<sockaddr*>(&a), sizeof(a));

        if (error) {
            throw std::runtime_error(get_last_error_message());
        }
    }


    //Connects the socket at the specified address.
    void socket::connect(const socket_address& addr) {
        sockaddr_in a{};
        a.sin_addr.S_un.S_addr = addr.address().value();
        a.sin_family = AF_INET;
        a.sin_port = htons(addr.port_number());

        int error = ::connect(handle(), reinterpret_cast<sockaddr*>(&a), sizeof(a));

        if (error) {
            throw std::runtime_error(get_last_error_message());
        }
    }


} //namespace netlib::ip4
