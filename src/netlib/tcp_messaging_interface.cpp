#include "netlib/tcp_messaging_interface.hpp"
#include "netlib/socket_error.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/message_size.hpp"
#include "internals/send_receive_message.hpp"


namespace netlib {


    //returns the appropriate socket type from address family.
    static socket::TYPE get_tcp_socket_type(int af) {

        //tcp on ip4
        if (af == constants::ADDRESS_FAMILY_IP4) {
            return socket::TYPE::TCP_IP4;
        }

        //tcp on ip6
        if (af == constants::ADDRESS_FAMILY_IP6) {
            return socket::TYPE::TCP_IP6;
        }

        //error
        throw socket_error(stringstream() << "Unsupported address family: " << af);
    }


    //the default constructor.
    tcp_messaging_interface::tcp_messaging_interface() {
    }


    //Constructor from address family.
    tcp_messaging_interface::tcp_messaging_interface(int af)
        : socket_messaging_interface(get_tcp_socket_type(af))
    {
    }


    //Constructor from tcp socket.
    tcp_messaging_interface::tcp_messaging_interface(socket&& tcp_socket) 
        : socket_messaging_interface(move_socket(tcp_socket, socket::SOCKET_STREAM))
    {
    }


    //Assignment from tcp socket.
    void tcp_messaging_interface::set_socket(socket&& tcp_socket) {
        socket_messaging_interface::set_socket(move_socket(tcp_socket, socket::SOCKET_STREAM));
    }


    //Opens the socket.
    void tcp_messaging_interface::open_socket(int af) {
        get_socket().open(get_tcp_socket_type(af));
    }


    //Sends a message.
    bool tcp_messaging_interface::send_message(message&& msg) {
        return internals::send_message(msg, [&](byte_buffer& buffer) {
            return send_data(buffer, [&](byte_buffer& buffer) {
                return get_socket().stream_send(buffer.data(), buffer.size());
            });
        });
    }


    //Receives a message.
    message_pointer<> tcp_messaging_interface::receive_message(std::pmr::memory_resource& memres, size_t max_message_size) {
        return internals::receive_message(memres, max_message_size, [&](byte_buffer& buffer) {
            return receive_data(buffer, [&](byte_buffer& buffer) {
                return get_socket().stream_receive(buffer.data(), buffer.size());
            });
        });
    }


    //sends the message size.
    bool tcp_messaging_interface::send_message_size(const byte_buffer& buffer) {
        message_size msg_size = buffer_size_to_message_size(buffer.size());
        switch_endianess(msg_size);
        return get_socket().stream_send(&msg_size, sizeof(msg_size));
    }


    //receives the message size and resizes the buffer accordingly
    bool tcp_messaging_interface::receive_message_size(byte_buffer& buffer) {
        message_size msg_size;
        if (!get_socket().stream_receive(&msg_size, sizeof(msg_size))) {
            return false;
        }
        switch_endianess(msg_size);
        buffer.resize(msg_size);
        return true;
    }


} //namespace netlib
