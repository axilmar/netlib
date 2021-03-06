#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/unencrypted_tcp_client_socket.hpp"
#include "netlib/numeric_cast.hpp"
#include "netlib/endianess.hpp"
#include "netlib/message_size_t.hpp"


namespace netlib::unencrypted::tcp {


    //send data
    static bool _send(uintptr_t handle, const char* d, int len) {
        do {
            //send
            int s = ::send(handle, d, len, 0);

            //success
            if (s >= 0) {
                d += s;
                len -= s;
                continue;
            }

            //if closed
            if (is_socket_closed_error(get_last_error_number())) {
                return false;
            }

            //error
            throw std::system_error(get_last_error_number(), std::system_category());

        } while (len > 0);

        return true;
    }


    //receive data
    static bool _receive(uintptr_t handle, char* d, int len) {
        do {
            //receive
            int s = recv(handle, d, len, 0);

            //success
            if (s > 0) {
                d += s;
                len -= s;
                continue;
            }

            //special case/if closed
            if (s == 0 || is_socket_closed_error(get_last_error_number())) {
                return false;
            }

            //error
            throw std::system_error(get_last_error_number(), std::system_category());

        } while (len > 0);

        return true;
    }


    //Constructor.
    client_socket::client_socket(const std::optional<socket_address>& this_addr, const socket_address& server_addr, bool reuse_address_and_port)
        : socket(::socket(server_addr.address_family(), SOCK_STREAM, IPPROTO_TCP))
    {
        //optionally reuse address/port
        if (reuse_address_and_port) {
            set_reuse_address_and_port();
        }

        //optionally bind the socket
        if (this_addr.has_value() && ::bind(handle(), reinterpret_cast<const sockaddr*>(this_addr.value().data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //connect the socket
        if (::connect(handle(), reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        message_size_t size = numeric_cast<message_size_t>(data.size());

        //send size
        set_endianess(size);
        if (!_send(handle(), reinterpret_cast<const char*>(&size), sizeof(size))) {
            return false;
        }

        //send data
        return _send(handle(), data.data(), numeric_cast<int>(data.size()));
    }


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data) {
        message_size_t size;

        //receive size
        if (!_receive(handle(), reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        set_endianess(size);

        //receive data
        data.resize(size);
        return _receive(handle(), data.data(), numeric_cast<int>(size));
    }


} //namespace netlib::tcp
