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
         * Not used.
         * @exception std::logic_error always thrown.
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

    private:
        socket_address m_receiver_address;
        std::shared_ptr<udp_messaging_interface> m_messaging_interface;
    };


} //namespace netlib


#endif //NETLIB_UDP_CLIENT_MESSAGING_INTERFACE_HPP
