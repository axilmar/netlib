#ifndef NETLIB_SOCKET_MESSAGING_INTERFACE_HPP
#define NETLIB_SOCKET_MESSAGING_INTERFACE_HPP


#include "messaging_interface.hpp"
#include "socket.hpp"


namespace netlib {


    /**
     * An messaging_interface which uses a socket to send/receive message data. 
     */
    class socket_messaging_interface : public messaging_interface, public socket {
    public:
        using socket::socket;
        using socket::operator =;

        /**
         * Constructor from socket.
         * @param src socket.
         */
        socket_messaging_interface(socket&& src);

        /**
         * Assignment from socket.
         * @param src socket.
         * @return reference to this.
         */
        socket_messaging_interface& operator = (socket&& src);

    protected:
        /**
         * Sends the data with the help of this socket.
         * @param buffer buffer with data to transmit.
         * @return true if the data were transmitted successfully, false if the socket was closed.
         */
        bool send_message_data(byte_buffer& buffer) final;

        /**
         * Receives the data with the help of this socket.
         * @param buffer buffer to put the data to.
         * @return true if the data were transmitted successfully, false if the socket was closed.
         */
        bool receive_message_data(byte_buffer& buffer) final;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_MESSAGING_INTERFACE_HPP
