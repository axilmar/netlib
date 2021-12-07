#include "platform.hpp"
#include "numeric_cast.hpp"
#include "netlib/ip6_tcp_server_socket.hpp"
#include "netlib/ip6_tcp_client_socket.hpp"


namespace netlib::ip6::tcp {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //bind and listen
    server_socket::server_socket(const socket_address& bind_addr, int backlog) 
        : ip6::socket(::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP))
    {
        bind(bind_addr);
        
        if (::listen(handle(), backlog <= 0 ? SOMAXCONN : backlog)) {
            throw std::runtime_error(get_last_error_message());
        }
    }


    //Accepts a connection from a client.
    server_socket::client server_socket::accept() {
        sockaddr_in6 a;
        int addrlen = sizeof(a);
        socket_handle sh = ::accept(handle(), reinterpret_cast<sockaddr*>(&a), &addrlen);

        if (sh < 0) {
            throw std::runtime_error(get_last_error_message());
        }

        return { sh, socket_address({reinterpret_cast<const address::bytes_type&>(a.sin6_addr), a.sin6_scope_id}, ntohs(a.sin6_port)) };
    }


} //namespace netlib::ip6::tcp