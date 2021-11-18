#include "netlib/udp_receiver_messaging_interface.hpp"


namespace netlib {


    //Constructor.
    udp_receiver_messaging_interface::udp_receiver_messaging_interface(socket_address&& addr, std::shared_ptr<udp_messaging_interface>&& messaging_interface)
        : m_receiver_address(std::move(addr))
        , m_messaging_interface(std::move(messaging_interface))
    {
    }


    //Sends a message to a specific address.
    bool udp_receiver_messaging_interface::send_message(message&& msg) {
        m_messaging_interface->set_receiver_address(m_receiver_address);
        return m_messaging_interface->send_message(std::move(msg));
    }


} //namespace netlib
