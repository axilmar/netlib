#ifndef NETLIB_IP6_TCP_SERVER_SOCKET_HPP
#define NETLIB_IP6_TCP_SERVER_SOCKET_HPP


#include "ip6_tcp_client_socket.hpp"


namespace netlib::ip6::tcp {


    /**
     * A tcp server socket. 
     */
    class server_socket : public ip6::socket {
    public:
        /**
         * Result of accept function. 
         */
        struct client {
            /**
             * client socket. 
             */
            class client_socket socket;

            /**
             * Address of client. 
             */
            socket_address address;
        };

        /**
         * The default constructor.
         * The socket is created in an invalid state.
         */
        server_socket() {
        }

        /**
         * Constructor that binds the socket to the given address and listens to it for connections.
         * @param bind_addr address to bind and listen to.
         * @param backlog max queue size; if 0 or less than 0, SOMAXCONN is used.
         * @exception std::runtime_error if there was an error.
         */
        server_socket(const socket_address& bind_addr, int backlog = 0);

        /**
         * The move constructor.
         * @param src source socket.
         */
        server_socket(client_socket&& src) : ip6::socket(std::move(src)) {
        }

        /**
         * The move assignment operator.
         * @param src source socket.
         * @return reference to this.
         */
        server_socket& operator = (server_socket&& src) {
            ip6::socket::operator = (std::move(src));
            return *this;
        }

        /**
         * Accepts a connection from a client.
         * @return client information.
         * @exception std::runtime_error if there was an error.
         */
        client accept();
    };


} //namespace netlib::ip6::tcp



#endif //NETLIB_IP6_TCP_SERVER_SOCKET_HPP
