#ifndef NETLIB_SSL_TCP_SERVER_SOCKET_HPP
#define NETLIB_SSL_TCP_SERVER_SOCKET_HPP


#include "ssl_tcp_server_context.hpp"
#include "ssl_tcp_client_socket.hpp"
#include "unencrypted_socket.hpp"


namespace netlib::ssl::tcp {


    /**
     * SSL TCP server socket.
     * The socket itself is an unencrypted socket, since it does not handle any data.
     */
    class server_socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         */
        server_socket() : unencrypted::socket() {
        }

        /**
         * Creates a socket, binds it to the given address, and listens for connections.
         * @param context context.
         * @param addr address to bind the socket to.
         * @param backlog backlog; if 0, SOMAXCONN is used.
         * @exception std::system_error thrown if there is a socket error.
         * @exception ssl_error thrown if there is an ssl error.
         */
        server_socket(const server_context& context, const socket_address& addr, int backlog = 0);

        /**
         * Accepts a socket connection.
         * @param addr client address.
         * @return client socket.
         * @exception std::system_error thrown if there is a socket error.
         * @exception ssl_error thrown if there is an ssl error.
         */
        std::shared_ptr<client_socket> accept(socket_address& addr);

    private:
        std::shared_ptr<ssl_ctx_st> m_ctx;
    };


} //namespace netlib::ssl::tcp


#endif //NETLIB_SSL_TCP_SERVER_SOCKET_HPP
