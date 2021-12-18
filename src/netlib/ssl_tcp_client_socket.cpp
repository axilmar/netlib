#include "platform.hpp"
#include <system_error>
#include "ssl.hpp"
#include "netlib/ssl_tcp_client_socket.hpp"
#include "netlib/numeric_cast.hpp"
#include "netlib/ssl_error.hpp"
#include "netlib/message_size_t.hpp"
#include "netlib/endianess.hpp"


namespace netlib::ssl::tcp {


    //create the socket and the ssl
    static std::shared_ptr<SSL> create_ssl(const client_context& context, const std::optional<socket_address>& this_addr, const socket_address& server_addr, bool reuse_address_and_port) {
        //create the socket
        socket::handle_type sock = ::socket(server_addr.address_family(), SOCK_STREAM, IPPROTO_TCP);

        //failure to create the socket
        if (sock < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //set reuse
        if (reuse_address_and_port) {
            socket::set_reuse_address_and_port(sock);
        }

        //optionally bind the socket
        if (this_addr.has_value() && ::bind(sock, reinterpret_cast<const sockaddr*>(this_addr.value().data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //connect to the server; if error, throw exception
        if (::connect(sock, reinterpret_cast<const sockaddr*>(server_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //crreate the ssl
        std::shared_ptr<SSL> ssl{SSL_new(context.ctx().get()), SSL_close};

        //connect the ssl and the socket
        SSL_set_fd(ssl.get(), numeric_cast<int>(sock));

        //connect the SLL part
        if (SSL_connect(ssl.get()) != 1) {
            throw ssl::error(ERR_get_error());
        }

        //ssl socket was successfully created
        return ssl;
    }


    //constructor
    client_socket::client_socket(const client_context& context, const std::optional<socket_address>& this_addr, const socket_address& server_addr, bool reuse_address_and_port)
        : ssl::socket(context.ctx(), create_ssl(context, this_addr, server_addr, reuse_address_and_port))
    {
    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        message_size_t size = numeric_cast<message_size_t>(data.size());

        //send size
        set_endianess(size);
        if (!ssl_send(ssl().get(), reinterpret_cast<const char*>(&size), sizeof(size))) {
            return false;
        }

        //send data
        return ssl_send(ssl().get(), data.data(), numeric_cast<int>(data.size()));
    }


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data) {
        message_size_t size;

        //receive size
        if (!ssl_receive(ssl().get(), reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        set_endianess(size);

        //receive data
        data.resize(size);
        return ssl_receive(ssl().get(), data.data(), numeric_cast<int>(size));
    }


} //namespace netlib::ssl::tcp
