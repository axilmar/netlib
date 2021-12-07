#include "platform.hpp"
#include "numeric_cast.hpp"
#include "netlib/ip6_udp_socket.hpp"


namespace netlib::ip6::udp {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //Creates and binds the socket.
    socket::socket(const socket_address& bind_addr)
        : ip6::socket(::socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP))
    {
        ip6::socket::bind(bind_addr);
    }


    //sends the given data to a specific address.
    size_t socket::send(const byte_buffer& buffer, const socket_address& addr) {
        //destination address
        sockaddr_in6 a{};
        a.sin6_addr = reinterpret_cast<const in6_addr&>(addr.address().bytes());
        a.sin6_family = AF_INET6;
        a.sin6_port = htons(addr.port_number());
        a.sin6_scope_id = addr.address().zone_index();

        //send data
        int bytes_sent = ::sendto(handle(), buffer.data(), numeric_cast<int>(buffer.size()), 0, reinterpret_cast<const sockaddr*>(&a), sizeof(a));

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
        sockaddr_in6 a;
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
        addr.set_address({ reinterpret_cast<const address::bytes_type&>(a.sin6_addr), a.sin6_scope_id });
        addr.set_port_number(ntohs(a.sin6_port));

        //return bytes received
        return bytes_received;
    }


} //namespace netlib::ip6::udp
