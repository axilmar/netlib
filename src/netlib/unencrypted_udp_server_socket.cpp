#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/unencrypted_udp_server_socket.hpp"
#include "netlib/numeric_cast.hpp"


namespace netlib::unencrypted::udp {


    //Constructor.
    server_socket::server_socket(const socket_address& server_addr) 
        : socket(::socket(server_addr.type(), SOCK_DGRAM, 0))
    {
        //bind
        if (::bind(handle(), reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
    }


    //mimic 'accept', as in tcp
    std::shared_ptr<client_socket> server_socket::accept(socket_address& client_addr) {
        //receive data
        RECEIVE:
        char buf;
        int fromlen = sizeof(sockaddr_storage);
        int s = recvfrom(handle(), &buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(client_addr.data()), &fromlen);

        //if error
        if (s <= 0) {
            if (is_socket_closed_error(get_last_error_number())) {
                return nullptr;
            }
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //if received byte is not 0, ignore the message
        if (buf != 0) {
            goto RECEIVE;
        }

        //create new socket for the connection
        handle_type client_socket = ::socket(client_addr.type(), SOCK_DGRAM, IPPROTO_UDP);

        //error creating the client socket
        if (client_socket < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //bind the client socket to another server address, same ip, random port
        socket_address bound_addr = bound_address();
        socket_address server_addr(bound_addr.address(), 0);
        if (::bind(client_socket, reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //connect the client socket to the client address so as that 
        //received messages are sent to the client socket,
        //and not to this server socket
        if (::connect(client_socket, reinterpret_cast<const sockaddr*>(client_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //send an acknowledgement to the client;
        //instead of sending an acknowledgement value, send the address the socket was bound to on this end,
        //allowing the client to connect the socket to this address
        server_addr = socket::bound_address(client_socket);
        ::send(client_socket, server_addr.data(), sizeof(sockaddr_storage), 0);

        //success; create client socket object
        return std::make_shared<udp::client_socket>(client_socket);
    }


} //namespace netlib::udp
