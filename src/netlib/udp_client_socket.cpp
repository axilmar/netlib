#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/udp_client_socket.hpp"
#include "numeric_cast.hpp"


namespace netlib::udp {


    //Constructor.
    client_socket::client_socket(const socket_address& addr) 
        : socket(::socket(addr.type(), SOCK_DGRAM, IPPROTO_UDP))
    {
        if (::connect(handle(), reinterpret_cast<const sockaddr*>(addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }
    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        //send
        int s = ::send(handle(), data.data(), numeric_cast<int>(data.size()), 0);

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


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data, uint16_t max_message_size) {
        //resize data for max udp packet size
        data.resize(max_message_size == 0 ? 65536 : max_message_size);

        //receive
        int s = ::recv(handle(), data.data(), numeric_cast<int>(data.size()), 0);

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
