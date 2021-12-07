#ifndef NETLIB_IP4_SOCKET_HPP
#define NETLIB_IP4_SOCKET_HPP


#include "ip4_socket_address.hpp"
#include "socket.hpp"


namespace netlib::ip4 {


    /**
     * Base class for ip4 sockets. 
     */
    class socket : public netlib::socket {
    public:
        /**
         * Returns the assigned address of the socket.
         * @return the assigned address of the socket.
         */
        socket_address get_assigned_address() const;

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


} //namespace netlib::ip4


#endif //NETLIB_IP4_SOCKET_HPP
