#ifndef NETLIB_UDP_CLIENT_SOCKET_HPP
#define NETLIB_UDP_CLIENT_SOCKET_HPP


#include <vector>
#include "socket.hpp"
#include "socket_address.hpp"


namespace netlib::udp {


    /**
     * UDP client socket.
     */
    class client_socket : public socket {
    public:
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


} //namespace netlib::udp


#endif //NETLIB_UDP_CLIENT_SOCKET_HPP
