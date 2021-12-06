#ifndef NETLIB_IP6_TCP_CLIENT_SOCKET_HPP
#define NETLIB_IP6_TCP_CLIENT_SOCKET_HPP


#include "ip6_socket.hpp"


namespace netlib::ip6::tcp {


    /**
     * A tcp client socket. 
     */
    class client_socket : public ip6::socket {
    public:
        /**
         * The default constructor.
         * The socket is created in an invalid state.
         */
        client_socket() {
        }

        /**
         * Constructor that connects the socket to the given address.
         * @param connect_addr address to connect to.
         * @exception std::runtime_error if there was an error.
         */
        client_socket(const socket_address& connect_addr);

        /**
         * The move constructor.
         * @param src source socket.
         */
        client_socket(client_socket&& src) : ip6::socket(std::move(src)) {
        }

        /**
         * The move assignment operator.
         * @param src source socket.
         * @return reference to this.
         */
        client_socket& operator = (client_socket&& src) {
            ip6::socket::operator = (std::move(src));
            return *this;
        }

        /**
         * Sends the data in the given buffer.
         * It sends all the data; if there is not enough space in the system buffer,
         * then it blocks until some is available.
         * @param buffer buffer with data to send.
         * @return size of given buffer.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t send(const byte_buffer& buffer);
        
        /**
         * Receives data into the given buffer.
         * @param buffer buffer to store the received data.
         * @param receive_size number of bytes to receive.
         * @return receive_size or 0 if the socket is gracefully closed.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t receive(byte_buffer& buffer, const size_t receive_size);

    private:
        //internal constructor for the server_socket class.
        client_socket(socket_handle handle) : ip6::socket(handle) {
        }

        friend class server_socket;
    };


} //namespace netlib::ip6::tcp


#endif //NETLIB_IP6_TCP_CLIENT_SOCKET_HPP
