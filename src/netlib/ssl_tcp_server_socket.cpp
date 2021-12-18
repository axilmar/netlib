#include "platform.hpp"
#include "ssl.hpp"
#include <system_error>
#include "netlib/ssl_tcp_server_socket.hpp"
#include "netlib/ssl_error.hpp"
#include "netlib/numeric_cast.hpp"


namespace netlib::ssl::tcp {


    //internal client socket
    class internal_client_socket : public client_socket {
    public:
        internal_client_socket(const std::shared_ptr<ssl_ctx_st>& ctx, const std::shared_ptr<ssl_st>& ssl) : client_socket(ctx, ssl) {}
    };


    //create the socket
    static unencrypted::socket::handle_type create_socket(const socket_address& this_addr, int backlog) {
        //create the socket
        socket::handle_type sock = ::socket(this_addr.address_family(), SOCK_STREAM, IPPROTO_TCP);

        //failure to create the socket
        if (sock < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //bind the socket; if error, throw exception
        if (::bind(sock, reinterpret_cast<const sockaddr*>(this_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //put socket in listen mode
        if (::listen(sock, backlog ? backlog : SOMAXCONN)) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //success
        return sock;
    }


    //Creates a socket, binds it to the given address, and listens for connections.
    server_socket::server_socket(const server_context& context, const socket_address& this_addr, int backlog)
        : unencrypted::socket(create_socket(this_addr, backlog))
        , m_ctx(context.ctx())
    {
    }


    //Accepts a socket connection.
    std::shared_ptr<client_socket> server_socket::accept(socket_address& addr) {
        //accept
        int addrlen = sizeof(sockaddr_storage);
        uintptr_t handle = ::accept(this->handle(), reinterpret_cast<sockaddr*>(addr.data()), &addrlen);

        //error
        if (handle < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //create ssl
        std::shared_ptr<SSL> ssl{SSL_new(m_ctx.get()), SSL_close};

        //bind the ssl and the client socket
        SSL_set_fd(ssl.get(), numeric_cast<int>(handle));

        //accept
        if (SSL_accept(ssl.get()) != 1) {
            throw ssl::error(ERR_get_error());
        }

        //success
        return std::make_shared<internal_client_socket>(m_ctx, ssl);
    }


} //namespace netlib::tcp
