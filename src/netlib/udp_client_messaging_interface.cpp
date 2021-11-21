#include "netlib/udp_client_messaging_interface.hpp"


namespace netlib {


    //Constructor.
    udp_client_messaging_interface::udp_client_messaging_interface(const socket_address& addr, const std::shared_ptr<udp_messaging_interface>& messaging_interface)
        : m_receiver_address(addr)
        , m_messaging_interface(messaging_interface)
    {
    }


    //Not used.
    bool udp_client_messaging_interface::send_message(message&& msg) {
        throw std::logic_error("invalid socket type for this operation");
    }


    //Sends a message.
    bool udp_client_messaging_interface::send_message(message&& msg, const address& addr) {
        return m_messaging_interface->send_message(std::move(msg), addr);
    }


} //namespace netlib
