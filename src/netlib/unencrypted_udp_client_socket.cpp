#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include <chrono>
#include "netlib/unencrypted_udp_client_socket.hpp"
#include "netlib/numeric_cast.hpp"


//define connection timeout if not connected
#ifndef NETLIB_UNENCRYPTED_UDP_CLIENT_CONNECTION_TIMEOUT
#define NETLIB_UNENCRYPTED_UDP_CLIENT_CONNECTION_TIMEOUT 10000
#endif 


namespace netlib::unencrypted::udp {


    //Constructor.
    client_socket::client_socket(const socket_address& server_addr, const socket_address& client_addr)
        : socket(::socket(server_addr.type(), SOCK_DGRAM, IPPROTO_UDP))
    {
        //bind this socket to an address
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(client_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        const auto connect_start_time = std::chrono::high_resolution_clock::now();

        //send a message to the server so as that the server can do an 'accept'
        SEND:
        char buf = 0;
        ::sendto(handle(), &buf, sizeof(buf), 0, reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage));

        //wait for server acknowledgment (actually, for the server's client socket ip address)
        POLL:
        pollfd fda;
        fda.events = POLLRDNORM;
        fda.fd = handle();
    
        int r = poll(&fda, 1, 100);

        //if error
        if (r < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //if timeout
        if (r == 0) {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - connect_start_time);

            //if the total connection duration has not been reached yet, retry sending
            if (dur.count() < NETLIB_UNENCRYPTED_UDP_CLIENT_CONNECTION_TIMEOUT) {
                goto SEND;
            }

            //connection timeout
            throw std::system_error(get_connection_timeout_error_number(), std::system_category());
        }

        //get the data
        socket_address server_client_addr;
        int s = recv(handle(), server_client_addr.data(), sizeof(sockaddr_storage), 0);

        //if error
        if (s < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //if another kind of packet was received, forget it
        if (s != sizeof(sockaddr_storage)) {
            goto POLL;
        }

        //now connect the local socket to the address provided to by the server
        if (::connect(handle(), reinterpret_cast<const sockaddr*>(server_client_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        if (data.size() == 1) {
            int x = 0;
        }

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
        throw std::system_error(get_last_error_number(), std::system_category());
    }


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data, uint16_t max_message_size) {
        //resize data for max udp packet size
        data.resize(max_message_size);

        //receive
        int s = ::recv(handle(), data.data(), numeric_cast<int>(data.size()), 0);

        //success
        if (s >= 0) {
            if (s == 1) {
                int x = 0;
            }
            data.resize(s);
            return true;
        }

        //if closed
        if (is_socket_closed_error(get_last_error_number())) {
            return false;
        }

        //error
        throw std::system_error(get_last_error_number(), std::system_category());
    }


} //namespace netlib::udp
