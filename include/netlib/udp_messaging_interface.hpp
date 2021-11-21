#ifndef NETLIB_UDP_MESSAGING_INTERFACE_HPP
#define NETLIB_UDP_MESSAGING_INTERFACE_HPP


#include "socket_messaging_interface.hpp"


namespace netlib {


    /**
     * UDP messaging interface.
     * It adds a crc32 on the end of each message.
     */
    class udp_messaging_interface : public socket_messaging_interface {
    public:
        /**
         * the default constructor.
         */
        udp_messaging_interface();

        /**
         * Constructor from address family.
         * @param af address family.
         */
        udp_messaging_interface(int af = constants::ADDRESS_FAMILY_IP4);

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
         * Sends a message.
         * @param msg message to send.
         * @param addr sender address; must be instance of socket_address.
         * @return true if the message was sent, false if it could not be sent.
         * @exception std::bad_cast thrown if the address is not a socket address.
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
         * Receives a message.
         * @param addr address variable to receive the sender address.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         * @exception std::bad_cast thrown if the address is not a socket address.
         */
        message_pointer<> receive_message(address& addr, std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) override;

        using socket_messaging_interface::receive_message;
    };


} //namespace netlib


#endif //NETLIB_UDP_MESSAGING_INTERFACE_HPP
