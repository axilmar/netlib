#ifndef NETLIB_UNENCRYPTED_UDP_SERVER_SOCKET_HPP
#define NETLIB_UNENCRYPTED_UDP_SERVER_SOCKET_HPP


#include <vector>
#include "unencrypted_udp_socket.hpp"


namespace netlib::unencrypted::udp {


    /**
     * UDP server socket.
     */
    class server_socket : public unencrypted::udp::socket {
    public:
        /**
         * The default constructor.
         * An invalid socket is created.
         */
        server_socket() : unencrypted::udp::socket() {
        }

        /**
         * Constructor from handle.
         * @param handle socket handle.
         * @exception std::system_error if the socket is invalid.
         */
        server_socket(handle_type handle) : unencrypted::udp::socket(handle) {
        }

        /**
         * Creates a socket, binds it to the given address.
         * @param this_addr address to bind the socket to.
         * @param reuse_addr_and_port if set, then SO_REUSEADDR and SO_REUSEPORT (if available) are set on the socket.
         * @exception std::system_error thrown if there is an error.
         */
        server_socket(const socket_address& this_addr, bool reuse_address_and_port = false) : unencrypted::udp::socket(this_addr, reuse_address_and_port) {
        }
    };


} //namespace netlib::unencrypted::udp


#endif //NETLIB_UNENCRYPTED_UDP_SERVER_SOCKET_HPP
