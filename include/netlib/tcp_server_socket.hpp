#ifndef NETLIB_TCP_SERVER_SOCKET_HPP
#define NETLIB_TCP_SERVER_SOCKET_HPP


#include "tcp_client_socket.hpp"


namespace netlib::tcp {


    /**
     * TCP server socket.
     */
    class server_socket : public socket {
    public:
        /**
         * The default constructor.
         * @param handle optional socket handle.
         */
        server_socket(handle_type handle = invalid_handle) : socket(handle) {
        }

        /**
         * Creates a socket, binds it to the given address, and listens for connections.
         * @param addr address to bind the socket to.
         * @param backlog backlog; if 0, SOMAXCONN is used.
         * @exception std::system_error thrown if there is an error.
         */
        server_socket(const socket_address& addr, int backlog = 0);

        /**
         * Accepts a socket connection.
         * @param addr client address.
         * @return client socket.
         * @exception std::system_error thrown if there is an error.
         */
        std::shared_ptr<client_socket> accept(socket_address& addr);
    };


} //namespace netlib::tcp


#endif //NETLIB_TCP_SERVER_SOCKET_HPP
