#ifndef NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP
#define NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP


#include <vector>
#include "unencrypted_socket.hpp"
#include "udp.hpp"


namespace netlib::unencrypted::udp {


    /**
     * UDP client socket.
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
         * @param this_addr address to bind this socket to.
         * @param server_addr address of server.
         * @param reuse_addr_and_port if set, then SO_REUSEADDR and SO_REUSEPORT (if available) are set on the socket.
         * @exception std::system_error if a system error has occurred.
         */
        client_socket(const socket_address& this_addr, const socket_address& server_addr, bool reuse_address_and_port = false);

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
        bool receive(std::vector<char>& data, const uint16_t max_message_size = NETLIB_UDP_MAX_MESSAGE_SIZE);
    }; 


} //namespace netlib::unencrypted::udp


#endif //NETLIB_UNENCRYPTED_UDP_CLIENT_SOCKET_HPP
