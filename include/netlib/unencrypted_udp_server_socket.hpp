#ifndef NETLIB_UNENCRYPTED_UDP_SERVER_SOCKET_HPP
#define NETLIB_UNENCRYPTED_UDP_SERVER_SOCKET_HPP


#include <vector>
#include "unencrypted_udp_client_socket.hpp"


namespace netlib::unencrypted::udp {


    /**
     * UDP server socket.
     */
    class server_socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         * @param handle optional socket handle.
         */
        server_socket(handle_type handle = invalid_handle) : unencrypted::socket(handle) {
        }

        /**
         * Constructor.
         * @param server_addr address to bind this server socket to.
         */
        server_socket(const socket_address& server_addr);

        /**
         * Accepts a socket connection.
         * 
         * The accept is faked in this manner:
         * when the server socket receives a connect message,
         * it creates a new socket on a random port and then sends its address to the client;
         * the client then connects its socket to the sent address.
         * This means that the number of actual udp client sockets allowed
         * is the number of random ports allowed.
         * 
         * The alternative, i.e. to have an unconnected udp socket acting as a server
         * and many connected udp sockets acting as client sockets, all in the same 
         * ip address and port, does not work on Windows, in which the behaviour
         * is that the last udp socket bound to a socket address receives
         * all the messages...on Linux it works, because for udp Linux
         * sends the packets for connected sockets to the actual connected sockets
         * and the packets for unconnected sockets to the connectionless sockets.
         * 
         * @param client_addr client address.
         * @return client socket.
         * @exception std::system_error thrown if there is an error.
         */
        std::shared_ptr<client_socket> accept(socket_address& client_addr);
    };


} //namespace netlib::unencrypted::udp


#endif //NETLIB_UDP_UNENCRYPTED_SERVER_SOCKET_HPP
