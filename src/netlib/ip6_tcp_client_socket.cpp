#include "platform.hpp"
#include "numeric_cast.hpp"
#include "netlib/ip6_tcp_client_socket.hpp"


namespace netlib::ip6::tcp {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    /**
     * Constructor that connects the socket to the given address.
     * @param connect_addr address to connect to.
     * @exception std::runtime_error if there was an error.
     */
    client_socket::client_socket(const socket_address& connect_addr)
        : ip6::socket(::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP))
    {
        connect(connect_addr);
    }


    //sends the data in the given buffer.
    size_t client_socket::send(const byte_buffer& buffer) {
        const char* data = buffer.data();
        int bytes_to_send = numeric_cast<int>(buffer.size());

        //send all the data
        do {
            int bytes_sent = ::send(handle(), data, bytes_to_send, 0);

            //if error, throw
            if (bytes_sent < 0) {
                throw std::runtime_error(get_last_error_message());
            }

            //next part
            bytes_to_send -= bytes_sent;
            data += bytes_sent;

        } while (bytes_to_send > 0);

        //return number of bytes sent
        return buffer.size();
    }


    //receives data into the given buffer.
    size_t client_socket::receive(byte_buffer& buffer, const size_t receive_size) {
        //make sure the buffer contains enough space
        buffer.resize(receive_size);

        char* data = buffer.data();
        int bytes_to_receive = numeric_cast<int>(receive_size);

        //receive all the data
        do {
            int bytes_received = ::recv(handle(), data, bytes_to_receive, 0);

            //if error, throw
            if (bytes_received < 0) {
                throw std::runtime_error(get_last_error_message());
            }

            //next part
            bytes_to_receive -= bytes_received;
            data += bytes_received;

        } while (bytes_to_receive > 0);

        //return number of bytes received
        return receive_size;
    }


} //namespace netlib::ip6::tcp
