#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/tcp_server_socket.hpp"


namespace netlib::tcp {


    //Creates a socket, binds it to the given address, and listens for connections.
    server_socket::server_socket(const socket_address& addr, int backlog)
        : socket(::socket(addr.type(), SOCK_STREAM, IPPROTO_TCP))
    {
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }

        if (::listen(handle(), backlog ? backlog : SOMAXCONN)) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }
    }


    //Accepts a socket connection.
    client_socket server_socket::accept(socket_address& addr) {
        //accept
        int addrlen = sizeof(sockaddr_storage);
        uintptr_t handle = ::accept(this->handle(), reinterpret_cast<sockaddr*>(addr.data()), &addrlen);

        //if no error
        if (handle >= 0) {
            return { handle };
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
    }


} //namespace netlib::tcp
