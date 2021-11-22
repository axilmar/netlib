#include "netlib/udp_messaging_interface.hpp"
#include "netlib/socket_error.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/crc32.hpp"
#include "netlib/internals/send_receive_message.hpp"


namespace netlib {


    static std::pmr::synchronized_pool_resource global_memory_pool;


    //returns the appropriate socket type from address family.
    static socket::TYPE get_udp_socket_type(int af) {

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
        : socket_messaging_interface(get_udp_socket_type(af))
    {
    }


    //Constructor from udp socket.
    udp_messaging_interface::udp_messaging_interface(socket&& udp_socket)
        : socket_messaging_interface(move_socket(udp_socket, socket::SOCKET_DATAGRAM))
    {
    }


    //Assignment from udp socket.
    void udp_messaging_interface::set_socket(socket&& udp_socket) {
        socket_messaging_interface::set_socket(move_socket(udp_socket, socket::SOCKET_DATAGRAM));
    }


    //Opens the socket.
    void udp_messaging_interface::open_socket(int af) {
        get_socket().open(get_udp_socket_type(af));
    }


    //Sends a message.
    bool udp_messaging_interface::send_message(message&& msg) {
        return internals::send_message(msg, [&](byte_buffer& buffer) {
            return send_packet(buffer, [&](const byte_buffer& buffer) {
                return send_data(buffer);
            });
        });
    }


    //Receives a message.
    message_pointer<> udp_messaging_interface::receive_message(std::pmr::memory_resource& memres, size_t max_message_size ) {
        return internals::receive_message(memres, max_message_size, [&](byte_buffer& buffer) {
            return receive_packet(buffer, [&](byte_buffer& buffer) {
                return receive_data(buffer);
            });
        });
    }


    //Sends a message to a specific address.
    bool udp_messaging_interface::send_message(message&& msg, const socket_address& addr) {
        return internals::send_message(msg, [&](byte_buffer& buffer) {
            return send_packet(buffer, [&](const byte_buffer& buffer) {
                return send_data(buffer, addr);
            });
        });
    }


    //Receives a message.
    message_pointer<> udp_messaging_interface::receive_message(socket_address& addr, std::pmr::memory_resource& memres, size_t max_message_size) {
        return internals::receive_message(memres, max_message_size, [&](byte_buffer& buffer) {
            return receive_packet(buffer, [&](byte_buffer& buffer) {
                return receive_data(buffer, addr);
            });
        });
    }


    //Sends the data.
    bool udp_messaging_interface::send_data(const byte_buffer& buffer) {
        return get_socket().send(buffer);
    }


    //Receives the data.
    bool udp_messaging_interface::receive_data(byte_buffer& buffer) {
        return get_socket().receive(buffer);
    }


    //Sends the data.
    bool udp_messaging_interface::send_data(const byte_buffer& buffer, const socket_address& addr) {
        return get_socket().send(buffer, addr);
    }


    //Receives the data.
    bool udp_messaging_interface::receive_data(byte_buffer& buffer, socket_address& addr) {
        return get_socket().receive(buffer, addr);
    }


    //Receives a message.
    message_pointer<> udp_messaging_interface::receive_message(socket_address& addr, size_t max_message_size) {
        return receive_message(addr, global_memory_pool, max_message_size);
    }


    //serialize crc32
    void udp_messaging_interface::serialize_crc32(byte_buffer& buffer) {
        const uint32_t crc32 = compute_crc32(buffer.data(), buffer.size());
        serialize(buffer, crc32);
    }


    //deserialize crc32 and compare it to the one stored in the data
    bool udp_messaging_interface::deserialize_crc32(byte_buffer& buffer) {
        const uint32_t received_crc32 = get_crc32(buffer);
        const uint32_t computed_crc32 = compute_crc32(buffer.data(), buffer.size() - sizeof(uint32_t));
        return received_crc32 == computed_crc32;
    }


} //namespace netlib
