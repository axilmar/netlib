#include "netlib/udp_messaging_interface.hpp"
#include "netlib/socket_error.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/crc32.hpp"


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


    //get crc32 stored in buffer
    static uint32_t get_crc32(const byte_buffer& buffer) {
        uint32_t buffer_crc32;
        copy_value(&buffer_crc32, reinterpret_cast<const uint32_t&>(buffer[buffer.size() - sizeof(uint32_t)]));
        return buffer_crc32;
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
        const uint32_t crc32 = compute_crc32(buffer.data(), buffer.size());
        serialize(crc32, buffer);
        return get_socket().send(buffer) == buffer.size();
    }


    //Receives the data.
    bool udp_messaging_interface::receive_data(byte_buffer& buffer) {
        const size_t size = get_socket().receive(buffer);
        if (size) {
            buffer.resize(size);
            const uint32_t received_crc32 = get_crc32(buffer);
            const uint32_t computed_crc32 = compute_crc32(buffer.data(), buffer.size() - sizeof(uint32_t));
            return received_crc32 == computed_crc32;
        }

        return false;
    }


} //namespace netlib
