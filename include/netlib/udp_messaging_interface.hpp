#ifndef NETLIB_UDP_MESSAGING_INTERFACE_HPP
#define NETLIB_UDP_MESSAGING_INTERFACE_HPP


#include "socket_messaging_interface.hpp"


namespace netlib {


    /**
     * UDP messaging interface.
     * On send_message, it supports an optional parameter of type const socket_address&, in order to send the data to a specific address;
     * On receive_message, it supports an optional parameter of type socket_address&, in order to receive the source address.
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

    protected:
        /**
         * Sends the data.
         * @param buffer buffer with data to transmit.
         * @param send_params accepts a const reference wrapper to a socket address;
         *  if this argument is specified, then the underlying call is send-to rather than send.
         * @return true if the data were sent successfully, false otherwise.
         */
        bool send_data(byte_buffer& buffer, const std::initializer_list<std::any>& send_params) override;

        /**
         * Receives the data.
         * @param buffer buffer to put the data to.
         * @param receive_params accepts a reference wrapper to a socket address;
         *  if this argument is specified, then the underlying call is receive-from rather than receive.
         * @return true if the data were received successfully, false otherwise.
         */
        bool receive_data(byte_buffer& buffer, const std::initializer_list<std::any>& send_params) override;
    };


} //namespace netlib


#endif //NETLIB_UDP_MESSAGING_INTERFACE_HPP
