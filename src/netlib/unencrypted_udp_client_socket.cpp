#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/unencrypted_udp_client_socket.hpp"
#include "netlib/numeric_cast.hpp"


namespace netlib::unencrypted::udp {


    //Constructor.
    client_socket::client_socket(const socket_address& this_addr, const socket_address& server_addr, bool reuse_address_and_port)
        : socket(::socket(this_addr.address_family(), SOCK_DGRAM, IPPROTO_UDP))
    {
        //optionally reuse address/port
        if (reuse_address_and_port) {
            set_reuse_address_and_port();
        }

        //bind the socket
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(this_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //connect the socket
        if (::connect(handle(), reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        int bytes = ::send(handle(), data.data(), numeric_cast<int>(data.size()), 0);

        if (bytes == data.size()) {
            return true;
        }

        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        throw std::system_error(get_last_error_number(), std::system_category());
    }


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data, const uint16_t max_message_size) {
        //resize the buffer to hold the max message size
        data.resize(max_message_size);

        //receive the data
        int bytes = ::recv(handle(), data.data(), static_cast<int>(data.size()), 0);

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
