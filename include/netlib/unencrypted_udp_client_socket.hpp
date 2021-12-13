#ifndef NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP
#define NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP


#include <vector>
#include "unencrypted_socket.hpp"


namespace netlib::unencrypted::udp {


    /**
     * UDP client socket.
     */
    class client_socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         * @param handle optional socket handle.
         */
        client_socket(handle_type handle = invalid_handle) : unencrypted::socket(handle) {
        }

        /**
         * Constructor.
         * @param addr address of server.
         */
        client_socket(const socket_address& addr);

        /**
         * Sends data to the server.
         * @param data data to send.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool send(const std::vector<char>& data);

        /**
         * Receives data from the server.
         * @param data reception buffer.
         * @param max_message_size number of bytes to allocate for the buffer. 
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool receive(std::vector<char>& data, uint16_t max_message_size = 65535);
    };


} //namespace netlib::unencrypted::udp


#endif //NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP
