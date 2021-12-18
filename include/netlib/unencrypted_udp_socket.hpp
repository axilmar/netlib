#ifndef NETLIB_UNENCRYPTED_UDP_SOCKET_HPP
#define NETLIB_UNENCRYPTED_UDP_SOCKET_HPP


#include <vector>
#include "unencrypted_socket.hpp"


namespace netlib::unencrypted::udp {


    /**
     * UDP socket.
     */
    class socket : public unencrypted::socket {
    public:
        /**
         * The default constructor.
         * An invalid socket is created.
         */
        socket() : unencrypted::socket() {
        }

        /**
         * Constructor from handle.
         * @param handle socket handle.
         * @exception std::system_error if the socket is invalid.
         */
        socket(handle_type handle) : unencrypted::socket(handle) {
        }

        /**
         * Constructor.
         * @param addr_family address family.
         * @param reuse_addr_and_port if set, then SO_REUSEADDR and SO_REUSEPORT (if available) are set on the socket.
         * @exception std::system_error if a system error has occurred.
         * @exception std::invalid_argument thrown if the address family is invalid.
         */
        socket(int addr_family, bool reuse_addr_and_port = false);

        /**
         * Constructor.
         * @param this_addr address to bind this to.
         * @param reuse_addr_and_port if set, then SO_REUSEADDR and SO_REUSEPORT (if available) are set on the socket.
         * @exception std::system_error if a system error has occurred.
         * @exception std::invalid_argument thrown if the address family is invalid.
         */
        socket(const socket_address& this_addr, bool reuse_addr_and_port = false);

        ///**
        // * Sends data to the connected peer.
        // * @param data data to send.
        // * @return true on success, false if the socket is closed.
        // * @exception std::system_error thrown if there was an error.
        // */
        //bool send(const std::vector<char>& data);

        ///**
        // * Receives data from the connected peer.
        // * @param data reception buffer.
        // * @param max_message_size number of bytes to allocate for the buffer.
        // * @return true on success, false if the socket is closed.
        // * @exception std::system_error thrown if there was an error.
        // */
        //bool receive(std::vector<char>& data, uint16_t max_message_size = 65535);

        /**
         * Sends data to the given address.
         * @param data data to send.
         * @param receiver_addr address to send the data to.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool send(const std::vector<char>& data, const socket_address& receiver_addr);

        /**
         * Receives data from the network.
         * @param data reception buffer.
         * @param sender_addr address of sender.
         * @param max_message_size number of bytes to allocate for the buffer.
         * @return true on success, false if the socket is closed.
         * @exception std::system_error thrown if there was an error.
         */
        bool receive(std::vector<char>& data, socket_address& sender_addr, uint16_t max_message_size = 65535);
    };


} //namespace netlib::unencrypted::udp


#endif //NETLIB_UNENCRYPTED_UDP_SOCKET_HPP
