#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include "network_address.hpp"


namespace netlib {


    class socket;


    /**
     * A socket address is a network address with a port.
     */
    class socket_address : public constants {
    public:
        /**
         * Creates an invalid socket address.
         */
        socket_address() {}

        /**
         * Creates a socket address from network address and port.
         * @param addr network address.
         * @param port port number.
         * @exception socket_error thrown if the network address is invalid.
         */
        socket_address(const network_address& addr, int port);

        /**
         * Returns the network address.
         * @return the network address.
         * @exception socket_error thrown if the socket address is invalid.
         */
        network_address get_address() const;

        /**
         * Returns the port.
         * @return the port.
         * @exception socket_error thrown if the socket address is invalid.
         */
        int get_port() const;

    private:
        //buffer size
        static constexpr size_t BUFFER_SIZE = 128;

        //internal buffer
        std::byte m_data[BUFFER_SIZE]{};

        friend class socket;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_ADDRESS_HPP
