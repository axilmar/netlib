#include "netlib/udp_client_messaging_interface.hpp"


namespace netlib {


    static thread_local byte_buffer thread_buffer;


    //Constructor.
    udp_client_messaging_interface::udp_client_messaging_interface(const socket_address& addr, const std::shared_ptr<udp_messaging_interface>& messaging_interface)
        : m_receiver_address(addr)
        , m_messaging_interface(messaging_interface)
    {
    }


    //Sends a message.
    bool udp_client_messaging_interface::send_message(message&& msg) {
        return m_messaging_interface->send_message(std::move(msg), m_receiver_address);
    }


} //namespace netlib
