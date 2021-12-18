#ifndef NETLIB_UNENCRYPTED_TCP_SERVER_SOCKET_HPP
#define NETLIB_UNENCRYPTED_TCP_SERVER_SOCKET_HPP


#include "unencrypted_tcp_client_socket.hpp"


namespace netlib::unencrypted::tcp {


    /**
     * TCP server socket.
     */
    class server_socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         * An invalid socket is created.
         */
        server_socket() : unencrypted::socket() {
        }

        /**
         * Constructor from handle.
         * @param handle socket handle.
         * @exception std::system_error if the socket is invalid.
         */
        server_socket(handle_type handle) : unencrypted::socket(handle) {
        }

        /**
         * Creates a socket, binds it to the given address, and listens for connections.
         * @param this_addr address to bind the socket to.
         * @param backlog backlog; if 0, SOMAXCONN is used.
         * @exception std::system_error thrown if there is an error.
         */
        server_socket(const socket_address& this_addr, int backlog = 0);

        /**
         * Accepts a socket connection.
         * @param addr client address.
         * @return client socket.
         * @exception std::system_error thrown if there is an error.
         */
        std::shared_ptr<client_socket> accept(socket_address& addr);
    };


} //namespace netlib::unencrypted::tcp


#endif //NETLIB_UNENCRYPTED_TCP_SERVER_SOCKET_HPP
