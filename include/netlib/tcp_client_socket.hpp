#ifndef NETLIB_TCP_CLIENT_SOCKET_HPP
#define NETLIB_TCP_CLIENT_SOCKET_HPP


#include <vector>
#include "socket.hpp"
#include "socket_address.hpp"


namespace netlib::tcp {


    /**
     * TCP client socket.
     */
    class client_socket : public socket {
    public:
        /**
         * The default constructor.
         * @param handle optional socket handle.
         */
        client_socket(handle_type handle = invalid_handle) : socket(handle) {
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
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool receive(std::vector<char>& data);
    }; 


} //namespace netlib::tcp


#endif //NETLIB_TCP_CLIENT_SOCKET_HPP
