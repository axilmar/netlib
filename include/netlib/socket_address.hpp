#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include "ip_address.hpp"


namespace netlib {


    class socket;


    /**
     * A socket address is a network address with a port.
     */
    class socket_address : public constants {
    public:
        /**
         * buffer size.
         */
        static constexpr size_t BUFFER_SIZE = 128;

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
        socket_address(const ip_address& addr, int port = 0);

        /**
         * Returns the network address.
         * @return the network address.
         * @exception socket_error thrown if the socket address is invalid.
         */
        ip_address get_address() const;

        /**
         * Returns the port.
         * @return the port.
         * @exception socket_error thrown if the socket address is invalid.
         */
        int get_port() const;

        /**
         * Returns pointer to internal data. 
         */
        const byte* data() const { return m_data; }

        /**
         * Returns pointer to internal data.
         */
        byte* data() { return m_data; }

        /**
         * Returns size of internal data. 
         */
        constexpr size_t size() const { return BUFFER_SIZE; }

    private:
        //internal buffer
        byte m_data[BUFFER_SIZE]{};
    };


} //namespace netlib


#endif //NETLIB_SOCKET_ADDRESS_HPP
