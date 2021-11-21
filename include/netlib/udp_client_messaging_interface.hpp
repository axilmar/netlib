#ifndef NETLIB_UDP_CLIENT_MESSAGING_INTERFACE_HPP
#define NETLIB_UDP_CLIENT_MESSAGING_INTERFACE_HPP


#include "udp_messaging_interface.hpp"


namespace netlib {


    /**
     * A sending messaging interface for a specific receiver.
     */
    class udp_client_messaging_interface : public sending_messaging_interface {
    public:
        /**
         * Constructor.
         * @param addr the receiver's address.
         * @param messaging_interface the udp sending interface to use for sending a message.
         */
        udp_client_messaging_interface(const socket_address& addr, const std::shared_ptr<udp_messaging_interface>& messaging_interface);

        /**
         * Sends a message to a specific address.
         * @param msg message to send.
         * @return true if the message was sent, false otherwise.
         */
        bool send_message(message&& msg) override;

    private:
        socket_address m_receiver_address;
        std::shared_ptr<udp_messaging_interface> m_messaging_interface;
    };


} //namespace netlib


#endif //NETLIB_UDP_CLIENT_MESSAGING_INTERFACE_HPP
