#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/unencrypted_tcp_server_socket.hpp"


namespace netlib::unencrypted::tcp {


    //Creates a socket, binds it to the given address, and listens for connections.
    server_socket::server_socket(const socket_address& this_addr, int backlog)
        : socket(::socket(this_addr.address_family(), SOCK_STREAM, IPPROTO_TCP))
    {
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(this_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        if (::listen(handle(), backlog ? backlog : SOMAXCONN)) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
    }


    //Accepts a socket connection.
    std::shared_ptr<client_socket> server_socket::accept(socket_address& addr) {
        //accept
        int addrlen = sizeof(sockaddr_storage);
        uintptr_t handle = ::accept(this->handle(), reinterpret_cast<sockaddr*>(addr.data()), &addrlen);

        //if no error
        if (handle >= 0) {
            return std::make_shared<client_socket>(handle);
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category());
    }


} //namespace netlib::tcp
