#include "platform.hpp"
#include <system_error>
#include "ssl.hpp"
#include "netlib/ssl_tcp_client_socket.hpp"
#include "netlib/numeric_cast.hpp"
#include "netlib/ssl_error.hpp"
#include "netlib/message_size_t.hpp"
#include "netlib/endianess.hpp"


namespace netlib::ssl::tcp {


    //send data
    static bool _send(SSL* ssl, const char* d, int len) {
        do {
            //send
            int s = SSL_write(ssl, d, len);

            //success
            if (s > 0) {
                d += s;
                len -= s;
                continue;
            }

            switch (ssl_handle_io_error(ssl, s)) {
            case ssl_io_result::success:
                return true;
            case ssl_io_result::failure:
                return false;
            }

        } while (len > 0);

        return true;
    }


    //receive data
    static bool _receive(SSL* ssl, char* d, int len) {
        do {
            //receive
            int s = SSL_read(ssl, d, len);

            //success
            if (s > 0) {
                d += s;
                len -= s;
                continue;
            }

            switch (ssl_handle_io_error(ssl, s)) {
            case ssl_io_result::success:
                return true;
            case ssl_io_result::failure:
                return false;
            }

        } while (len > 0);

        return true;
    }


    //create the socket and the ssl
    static std::shared_ptr<SSL> create_ssl(const client_context& context, const socket_address& server_addr) {
        //create the socket
        socket::handle_type sock = ::socket(server_addr.type(), SOCK_STREAM, IPPROTO_TCP);

        //failure to create the socket
        if (sock < 0) {
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
    client_socket::client_socket(const client_context& context, const socket_address& server_addr)
        : ssl::socket(context.ctx(), create_ssl(context, server_addr))
    {
    }


    //Sends data to the server.
    bool client_socket::send(const std::vector<char>& data) {
        message_size_t size = numeric_cast<message_size_t>(data.size());

        //send size
        set_endianess(size);
        if (!_send(ssl().get(), reinterpret_cast<const char*>(&size), sizeof(size))) {
            return false;
        }

        //send data
        return _send(ssl().get(), data.data(), numeric_cast<int>(data.size()));
    }


    //Receives data from the server.
    bool client_socket::receive(std::vector<char>& data) {
        message_size_t size;

        //receive size
        if (!_receive(ssl().get(), reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        set_endianess(size);

        //receive data
        data.resize(size);
        return _receive(ssl().get(), data.data(), numeric_cast<int>(size));
    }


} //namespace netlib::ssl::tcp
