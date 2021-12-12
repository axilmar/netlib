#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/udp_server_socket.hpp"
#include "numeric_cast.hpp"


namespace netlib::udp {


    //Constructor.
    server_socket::server_socket(const socket_address& addr) 
        : socket(::socket(addr.type(), SOCK_DGRAM, IPPROTO_UDP))
    {
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }
    }


    //Sends data.
    bool server_socket::send(const std::vector<char>& data, const socket_address& dst) const {
        //send
        int s = ::sendto(handle(), data.data(), numeric_cast<int>(data.size()), 0, reinterpret_cast<const sockaddr*>(dst.data()), sizeof(sockaddr_storage));

        //success
        if (s == data.size()) {
            return true;
        }

        //if closed
        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
    }


    //Receives data.
    bool server_socket::receive(std::vector<char>& data, socket_address& src, uint16_t max_message_size) const {
        //resize data for max udp packet
        data.resize(max_message_size);

        //send
        int srclen = sizeof(sockaddr_storage);
        int s = ::recvfrom(handle(), data.data(), numeric_cast<int>(data.size()), 0, reinterpret_cast<sockaddr*>(src.data()), &srclen);

        //success
        if (s >= 0) {
            data.resize(s);
            return true;
        }

        //if closed
        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
    }


} //namespace netlib::udp
