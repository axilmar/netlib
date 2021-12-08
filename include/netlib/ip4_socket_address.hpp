#ifndef NETLIB_IP4_SOCKET_ADDRESS_HPP
#define NETLIB_IP4_SOCKET_ADDRESS_HPP


#include "ip4_address.hpp"
#include "port_number.hpp"


namespace netlib::ip4 {


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
        socket_address(const ip4::address& addr, port_number port = 0)
            : m_address(addr)
            , m_port_number(port)
        {
        }

        /**
         * Returns the address.
         * @return the address.
         */
        const ip4::address& address() const {
            return m_address;
        }

        /**
         * Sets the address.
         * @param addr the address.
         */
        void set_address(const ip4::address& addr) {
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

        /**
         * Converts this socket address to string.
         */
        std::string to_string() const {
            return m_address.to_string() + ':' + std::to_string(m_port_number);
        }

        /**
         * Compares this object with the given one.
         * @param other the other object to compare this to.
         * @return -1 if this is less than the given object, 0 if they are equal, 1 if this is greater than the given object.
         */
        int compare(const socket_address& other) const;

        /**
         * Checks if this and the given object are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const socket_address& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if this and the given object are diferent.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const socket_address& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object comes before the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const socket_address& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object comes after the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const socket_address& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object comes before the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const socket_address& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object comes after the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const socket_address& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hashcode of this address.
         */
        size_t hash() const;

    private:
        //address and port number.
        ip4::address m_address;
        netlib::port_number m_port_number{};
    };


} //namespace netlib::ip4


namespace std {
    template <> struct hash<netlib::ip4::socket_address> {
        size_t operator ()(const netlib::ip4::socket_address& addr) const {
            return addr.hash();
        }
    };
}


#endif //NETLIB_IP4_SOCKET_ADDRESS_HPP
