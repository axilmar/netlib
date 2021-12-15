#ifndef NETLIB_SSL_TCP_CLIENT_SOCKET_HPP
#define NETLIB_SSL_TCP_CLIENT_SOCKET_HPP


#include "ssl_socket.hpp"
#include "ssl_tcp_client_context.hpp"


namespace netlib::ssl::tcp {


    /**
     * Client socket.
     */
    class client_socket : public ssl::socket {
    public:
        /**
         * Empty client socket constructor.
         */
        client_socket() : ssl::socket() {
        }

        /**
         * Constructor.
         * It connects to the server both in the socket layer and in the ssl layer.
         * @param context context for creating the ssl object.
         * @param server_addr server to connect to.
         * @exception std::system_error thrown if there is a system error.
         * @exception ssl_error thrown if there is an ssl error.
         */
        client_socket(const client_context& context, const socket_address& server_addr);

        /**
         * Sends data to the server.
         * @param data data to send.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         * @exception ssl_error thrown if there is an ssl error.
         * @exception bad_narrow_cast thrown if the buffer contains more bytes than what message_size_t can store.
         */
        bool send(const std::vector<char>& data);

        /**
         * Receives data from the server.
         * @param data reception buffer.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         * @exception ssl_error thrown if there is an ssl error.
         */
        bool receive(std::vector<char>& data);

    private:
        //constructor from server_socket::accept().
        client_socket(const std::shared_ptr<ssl_ctx_st>& ctx, const std::shared_ptr<ssl_st>& ssl) : ssl::socket(ctx, ssl) {}

        friend class internal_client_socket;
    };


} //namespace netlib::ssl::tcp


#endif //NETLIB_SSL_TCP_CLIENT_SOCKET_HPP
