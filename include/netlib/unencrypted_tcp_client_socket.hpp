#ifndef NETLIB_UNENCRYPTED_TCP_CLIENT_SOCKET_HPP
#define NETLIB_UNENCRYPTED_TCP_CLIENT_SOCKET_HPP


#include <vector>
#include <optional>
#include "unencrypted_socket.hpp"


namespace netlib::unencrypted::tcp {


    /**
     * TCP client socket.
     */
    class client_socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         * An invalid socket is created.
         */
        client_socket() : unencrypted::socket() {
        }

        /**
         * Constructor from handle.
         * @param handle socket handle.
         * @exception std::system_error if the socket is invalid.
         */
        client_socket(handle_type handle) : unencrypted::socket(handle) {
        }

        /**
         * Constructor.
         * @param this_addr address to optionally bind this socket to.
         * @param server_addr address of server.
         * @param reuse_addr_and_port if set, then SO_REUSEADDR and SO_REUSEPORT (if available) are set on the socket.
         * @exception std::system_error if a system error has occurred.
         */
        client_socket(const std::optional<socket_address>& this_addr, const socket_address& server_addr, bool reuse_address_and_port = false);

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
