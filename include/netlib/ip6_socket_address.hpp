#ifndef NETLIB_IP6_SOCKET_ADDRESS_HPP
#define NETLIB_IP6_SOCKET_ADDRESS_HPP


#include "ip6_address.hpp"
#include "port_number.hpp"


namespace netlib::ip6 {


    /**
     * Socket address. 
     */
    class socket_address {
    public:
        /**
         * The default constructor. 
         */
        socket_address() {
        }

        /**
         * Constructor from address and port.
         * @param addr address.
         * @param port port number.
         */
        socket_address(const ip6::address& addr, port_number port = 0)
            : m_address(addr)
            , m_port_number(port)
        {
        }

        /**
         * Returns the address.
         * @return the address.
         */
        const ip6::address& address() const {
            return m_address;
        }

        /**
         * Sets the address.
         * @param addr the address.
         */
        void set_address(const ip6::address& addr) {
            m_address = addr;
        }

        /**
         * Returns the port number.
         * @return the port number.
         */
        netlib::port_number port_number() const {
            return m_port_number;
        }

        /**
         * Sets the port number.
         * @param port the port number.
         */
        void set_port_number(netlib::port_number port) {
            m_port_number = port;
        }

    private:
        //address and port number.
        ip6::address m_address;
        netlib::port_number m_port_number{};
    };


} //namespace netlib::ip6


#endif //NETLIB_IP6_SOCKET_ADDRESS_HPP
