#include "platform.hpp"
#include "numeric_cast.hpp"
#include "netlib/ip4_udp_socket.hpp"


namespace netlib::ip4::udp {


    //Creates and binds the socket.
    socket::socket(const socket_address& bind_addr)
        : ip4::socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
    {
        ip4::socket::bind(bind_addr);
    }


    //sends the given data to a specific address.
    size_t socket::send(const byte_buffer& buffer, const socket_address& addr) {
        //destination address
        sockaddr_in a{};
        a.sin_addr.S_un.S_addr = addr.address().value();
        a.sin_family = AF_INET;
        a.sin_port = htons(addr.port_number());

        //send data
        int bytes_sent = ::send(handle(), buffer.data(), numeric_cast<int>(buffer.size()), 0);

        //if there was an error, throw
        if (bytes_sent < 0) {
            throw std::runtime_error(get_last_error_message());
        }

        //return bytes sent
        return bytes_sent;
    }


    //receives data from the network.
    size_t socket::receive(byte_buffer& buffer, socket_address& addr, const size_t max_packet_size) {
        //from address
        sockaddr_in a;
        int fromlen = sizeof(a);

        //make sure the buffer has enough size
        buffer.resize(max_packet_size);

        //receive the data
        int bytes_received = ::recvfrom(handle(), buffer.data(), numeric_cast<int>(buffer.size()), 0, reinterpret_cast<sockaddr*>(&a), &fromlen);

        //if there was an error, throw
        if (bytes_received < 0) {
            throw std::runtime_error(get_last_error_message());
        }

        //make sure the buffer does not have any data beyond those received
        buffer.resize(bytes_received);

        //store the from address
        addr.set_address(a.sin_addr.S_un.S_addr);
        addr.set_port_number(ntohs(a.sin_port));

        //return bytes received
        return bytes_received;
    }


} //namespace netlib::ip4::udp
