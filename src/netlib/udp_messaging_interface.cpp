#include "netlib/udp_messaging_interface.hpp"
#include "netlib/socket_error.hpp"
#include "netlib/serialization.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/crc32.hpp"


namespace netlib {


    //internal addresses
    static thread_local socket_address temp_address;
    static thread_local socket_address* temp_address_ptr{};


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


    //Opens the socket.
    void udp_messaging_interface::open_socket(int af) {
        get_socket().open(get_udp_socket_type(af));
    }


    //Sets the receiver address for the next send_message call.
    void udp_messaging_interface::set_receiver_address(socket_address& addr) {
        temp_address_ptr = &addr;
    }


    //Sets the receiver address for the next send_message call.
    void udp_messaging_interface::set_receiver_address(const std::any& addr) {
        temp_address = std::any_cast<socket_address>(addr);
    }


    //Returns the sender address from the last receive_message call.
    std::any udp_messaging_interface::get_sender_address() {
        return temp_address;
    }


    //Sends the data.
    bool udp_messaging_interface::send_data(byte_buffer& buffer) {
        //add crc32 to the message
        const uint32_t crc32 = compute_crc32(buffer.data(), buffer.size());
        serialize(buffer, crc32);

        size_t sent_size;

        //if socket is connected, ignore any sender address
        if (is_socket_connected()) {
            sent_size = get_socket().send(buffer);
            temp_address_ptr = nullptr;
        }

        //else if temp address is defined by pointer, use that
        else if (temp_address_ptr) {
            sent_size = get_socket().send(buffer, *temp_address_ptr);
            temp_address_ptr = nullptr;
        }

        //else use the temp_address instance
        else {
            sent_size = get_socket().send(buffer, temp_address);
        }

        //the call was ok if all the data sent
        return sent_size == buffer.size();
    }


    //Receives the data.
    bool udp_messaging_interface::receive_data(byte_buffer& buffer) {
        //receive data
        const size_t size = get_socket().receive(buffer, temp_address);

        //if successfully received data, compute crc32 and compare it with the one stored in the message
        if (size) {
            buffer.resize(size);
            const uint32_t received_crc32 = get_crc32(buffer);
            const uint32_t computed_crc32 = compute_crc32(buffer.data(), size - sizeof(uint32_t));
            return received_crc32 == computed_crc32;
        }

        return false;
    }


} //namespace netlib
