#ifndef NETLIB_IP4_UDP_SOCKET_HPP
#define NETLIB_IP4_UDP_SOCKET_HPP


#include "ip4_socket_address.hpp"
#include "ip4_socket.hpp"
#include "udp_constants.hpp"


namespace netlib::ip4::udp {


    /**
     * A udp socket. 
     */
    class socket : public netlib::ip4::socket {
    public:
        /**
         * The default constructor.
         * The socket is created in an invalid state.
         */
        socket() : ip4::socket() {
        }

        /**
         * Creates the socket.
         * @param bind_addr address to bind this socket to.
         * @exception std::runtime_error thrown if there is an error.
         */
        socket(const socket_address& bind_addr);

        /**
         * The move constructor.
         * @param src source socket.
         */
        socket(socket&& src) : ip4::socket(std::move(src)) {
        }

        /**
         * The move assignment operator.
         * @param src source socket.
         * @return reference to this.
         */
        socket& operator = (socket&& src) {
            ip4::socket::operator = (std::move(src));
            return *this;
        }

        /**
         * Sends the given data to a specific address. 
         * @param buffer buffer with data to send.
         * @param addr destination address.
         * @return number of bytes sent.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t send(const byte_buffer& buffer, const socket_address& addr);

        /**
         * Receives data from the network.
         * @param buffer buffer with data to receive; on output, it contains the received bytes.
         * @param addr source address.
         * @param max_packet_size max size of packet; the given buffer is resized to this value on entrance.
         * @return number of bytes received.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t receive(byte_buffer& buffer, socket_address& addr, const size_t max_packet_size = udp_max_packet_size);
    };


} //namespace netlib::ip4::udp


#endif //NETLIB_IP4_UDP_SOCKET_HPP
