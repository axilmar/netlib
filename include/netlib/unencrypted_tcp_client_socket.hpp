#ifndef NETLIB_UNENCRYPTED_TCP_CLIENT_SOCKET_HPP
#define NETLIB_UNENCRYPTED_TCP_CLIENT_SOCKET_HPP


#include <vector>
#include "unencrypted_socket.hpp"


namespace netlib::unencrypted::tcp {


    /**
     * TCP client socket.
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
         * @exception bad_narrow_cast thrown if the buffer contains more bytes than what message_size_t can store.
         */
        bool send(const std::vector<char>& data);

        /**
         * Receives data from the server.
         * @param data reception buffer.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool receive(std::vector<char>& data);
    }; 


} //namespace netlib::unencrypted::tcp


#endif //NETLIB_UNENCRYPTED_TCP_CLIENT_SOCKET_HPP
