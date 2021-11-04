#include "netlib/udp_messaging_interface.hpp"
#include "netlib/socket_error.hpp"
#include "netlib/stringstream.hpp"


namespace netlib {


    //returns the appropriate socket type from address family.
    static socket::TYPE get_socket_type(int af) {

        //udp on ip4
        if (af == constants::ADDRESS_FAMILY_IP4) {
            return socket::TYPE::UDP_IP4;
        }

        //udp on ip6
        if (af == constants::ADDRESS_FAMILY_IP6) {
            return socket::TYPE::UDP_IP6;
        }

        //error
        throw socket_error(stringstream() << "Unsupported address family: " << af);
    }


    //the default constructor.
    udp_messaging_interface::udp_messaging_interface() {
    }


    //Constructor from address family.
    udp_messaging_interface::udp_messaging_interface(int af)
        : socket_messaging_interface(get_socket_type(af))
    {
    }


    //Opens the socket.
    void udp_messaging_interface::open_socket(int af) {
        get_socket().open(get_socket_type(af));
    }


    //Sends the data.
    bool udp_messaging_interface::send_data(byte_buffer& buffer) {
        return get_socket().send(buffer) == buffer.size();
    }


    //Receives the data.
    bool udp_messaging_interface::receive_data(byte_buffer& buffer) {
        const size_t size = get_socket().receive(buffer);
        if (size) {
            buffer.resize(size);
            return true;
        }
        return false;
    }


} //namespace netlib
