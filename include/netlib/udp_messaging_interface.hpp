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
         * Sets the receiver address.
         * If set, then all packets are sent to this address.
         * @param addr address to send the message to.
         */
        void set_receiver_address(const socket_address& addr);

        /**
         * Returns the sender address from the last receive_message call of this thread.
         * @return the sender address.
         */
        const socket_address& get_sender_address();

    protected:
        /**
         * Sends the data.
         * It appends a crc32 code of the message at its end.
         * @param buffer buffer with data to transmit.
         * @return true if the data were sent successfully, false otherwise.
         */
        bool send_data(byte_buffer& buffer) override;

        /**
         * Receives the data.
         * @param buffer buffer to put the data to.
         * @return true if the data were received successfully and the crc32 was ok, false otherwise.
         */
        bool receive_data(byte_buffer& buffer) override;

    private:
        socket_address m_receiver_address;
        socket_address m_sender_address;
    };


} //namespace netlib


#endif //NETLIB_UDP_MESSAGING_INTERFACE_HPP
