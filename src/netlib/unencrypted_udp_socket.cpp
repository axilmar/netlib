#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/unencrypted_udp_socket.hpp"
#include "netlib/numeric_cast.hpp"


namespace netlib::unencrypted::udp {


    //constructor
    socket::socket(int addr_family, bool reuse_addr_and_port)
        : unencrypted::socket(::socket(addr_family, SOCK_DGRAM, IPPROTO_UDP))
    {
        //the address family must be valid
        if (addr_family != AF_INET && addr_family != AF_INET6) {
            throw std::invalid_argument("Invalid address family.");
        }

        //set reuse
        if (reuse_addr_and_port) {
            set_reuse_address_and_port();
        }
    }


    //constructor
    socket::socket(const socket_address& this_addr, bool reuse_addr_and_port)
        : unencrypted::socket(::socket(this_addr.address_family(), SOCK_DGRAM, IPPROTO_UDP))
    {
        //set reuse
        if (reuse_addr_and_port) {
            set_reuse_address_and_port();
        }

        //bind the socket
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(this_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

    }


    ////Sends data to the connected peer.
    //bool socket::send(const std::vector<char>& data) {
    //    //sent
    //    int bytes = ::send(handle(), data.data(), numeric_cast<int>(data.size()), 0);

    //    //sent ok
    //    if (bytes == data.size()) {
    //        return true;
    //    }

    //    //socket closed
    //    if (is_socket_closed_error(get_last_error_number())) {
    //        return false;
    //    }

    //    //error
    //    throw std::system_error(get_last_error_number(), std::system_category());
    //}


    ////Receives data from the connected peer.
    //bool socket::receive(std::vector<char>& data, uint16_t max_message_size) {
    //    data.resize(max_message_size);

    //    //receive
    //    int bytes = ::recv(handle(), data.data(), numeric_cast<int>(data.size()), 0);

    //    //receive ok
    //    if (bytes >= 0) {
    //        data.resize(bytes);
    //        return true;
    //    }

    //    //socket closed
    //    if (is_socket_closed_error(get_last_error_number())) {
    //        return false;
    //    }

    //    //error
    //    throw std::system_error(get_last_error_number(), std::system_category());
    //}


    //Sends data to the given address.
    bool socket::send(const std::vector<char>& data, const socket_address& receiver_addr) {
        //sent
        int bytes = ::sendto(handle(), data.data(), numeric_cast<int>(data.size()), 0, reinterpret_cast<const sockaddr*>(receiver_addr.data()), sizeof(sockaddr_storage));

        //sent ok
        if (bytes == data.size()) {
            return true;
        }

        //socket closed
        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category());
    }


    //Receives data from the network.
    bool socket::receive(std::vector<char>& data, socket_address& sender_addr, uint16_t max_message_size) {
        data.resize(max_message_size);

        //receive
        int fromlen = sizeof(socket_address);
        int bytes = ::recvfrom(handle(), data.data(), numeric_cast<int>(data.size()), 0, reinterpret_cast<sockaddr*>(sender_addr.data()), &fromlen);

        //receive ok
        if (bytes >= 0) {
            data.resize(bytes);
            return true;
        }

        //socket closed
        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category());
    }


} //namespace netlib::udp
