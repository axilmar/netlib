#ifndef NETLIB_IP6_SOCKET_HPP
#define NETLIB_IP6_SOCKET_HPP


#include "ip6_socket_address.hpp"
#include "socket.hpp"


namespace netlib::ip6 {


    /**
     * Base class for ip6 sockets. 
     */
    class socket : public netlib::socket {
    protected:
        /**
         * The default constructor.
         */
        socket() : netlib::socket() {
        }

        /**
         * Constructor from handle.
         * @param handle socket handle.
         */
        socket(socket_handle handle) : netlib::socket(handle) {
        }

        /**
         * The move constructor.
         * @param src the source object.
         */
        socket(socket&& src) : netlib::socket(std::move(src)) {
        }

        /**
         * The move assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src) {
            netlib::socket::operator = (std::move(src));
            return *this;
        }

        /**
         * Binds the socket at the specified address.
         * @param addr address to bind the socket to.
         * @exception std::runtime_error thrown if there is an error.
         */
        void bind(const socket_address& addr);

        /**
         * Connects the socket at the specified address.
         * @param addr address to connect the socket to.
         * @exception std::runtime_error thrown if there is an error.
         */
        void connect(const socket_address& addr);
    };


} //namespace netlib::ip6


#endif //NETLIB_IP6_SOCKET_HPP
