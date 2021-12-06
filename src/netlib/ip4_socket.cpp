#include "platform.hpp"
#include "netlib/ip4_socket.hpp"


namespace netlib::ip4 {


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
