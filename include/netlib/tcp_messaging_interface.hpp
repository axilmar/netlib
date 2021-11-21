#ifndef NETLIB_TCP_MESSAGING_INTERFACE_HPP
#define NETLIB_TCP_MESSAGING_INTERFACE_HPP


#include "socket_messaging_interface.hpp"


namespace netlib {


    /**
     * TCP messaging interface.
     */
    class tcp_messaging_interface : public socket_messaging_interface {
    public:
        /**
         * the default constructor.
         */
        tcp_messaging_interface();

        /**
         * Constructor from address family.
         * @param af address family.
         */
        tcp_messaging_interface(int af = constants::ADDRESS_FAMILY_IP4);

        /**
         * Constructor from tcp socket.
         * @aram tcp_socket socket.
         * @exception socket_error thrown if the socket is not tcp.
         */
        tcp_messaging_interface(socket&& tcp_socket);

        /**
         * Assignment from tcp socket.
         * @param tcp_socket socket.
         * @exception socket_error thrown if the socket is not tcp.
         */
        void set_socket(socket&& tcp_socket);

        /**
         * Opens the socket.
         * @param af address family.
         */
        void open_socket(int af = constants::ADDRESS_FAMILY_IP4);

        /**
         * Sends a message.
         * @param msg message to send.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(message&& msg) override;

        /**
         * Not used.
         * @exception std::logic_error always thrown.
         */
        bool send_message(message&& msg, const address& addr) override;

        /**
         * Receives a message.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) override;

        /**
         * Not used.
         * @exception std::logic_error always thrown.
         */
        message_pointer<> receive_message(address& addr, std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) override;

        using socket_messaging_interface::receive_message;
    };


} //namespace netlib


#endif //NETLIB_TCP_MESSAGING_INTERFACE_HPP
