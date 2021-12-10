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
         */
        client_socket() : socket() {
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

    private:
        client_socket(uintptr_t handle) : socket(handle) {}
        friend class server_socket;
    }; 


} //namespace netlib::tcp


#endif //NETLIB_TCP_CLIENT_SOCKET_HPP
