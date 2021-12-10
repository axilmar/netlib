#ifndef NETLIB_UDP_SERVER_SOCKET_HPP
#define NETLIB_UDP_SERVER_SOCKET_HPP


#include <vector>
#include "socket.hpp"
#include "socket_address.hpp"


namespace netlib::udp {


    /**
     * UDP server socket.
     */
    class server_socket : public socket {
    public:
        /**
         * The default constructor.
         */
        server_socket() : socket() {
        }

        /**
         * Constructor.
         * @param addr address to bind this server socket to.
         */
        server_socket(const socket_address& addr);

        /**
         * Sends data.
         * @param data data to send.
         * @param dst receiver address.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool send(const std::vector<char>& data, const socket_address& dst);

        /**
         * Receives data.
         * @param data reception buffer.
         * @param src sender address.
         * @param max_message_size maximum message size; if 0, then the max size is used.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool receive(std::vector<char>& data, socket_address& src, uint16_t max_message_size = 0);
    };


} //namespace netlib::udp


#endif //NETLIB_UDP_SERVER_SOCKET_HPP
