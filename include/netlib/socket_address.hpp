#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include "ip_address.hpp"


namespace netlib {


    /**
     * Socket address.
     */
    class socket_address {
    public:
        /**
         * The default constructor.
         * It constructs an IP4 address set to 0.
         */
        socket_address();

        /**
         * Constructs a socket address from the given ip address and port number.
         */
        socket_address(const ip_address& addr, uint16_t port = 0);

        /**
         * Returns the address type.
         */
        int address_type() const;

        /**
         * Returns the address.
         */
        ip_address address() const;

        /**
         * Returns the port.
         */
        uint16_t port() const;

        /**
         * Returns the data.
         */
        const void* data() const {
            return m_data.data();
        }

        /**
         * Converts the address to string.
         */
        std::string to_string() const;

        /**
         * Compares this to the given object.
         * @return other the object to compare this to.
         * @return less than zero if this comes before the given object,
         *  greater than zero if this comes after the given object,
         *  or zero if the objects are equal.
         */
        int compare(const socket_address& other) const;

        /**
         * Checks if the two objects are equal.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const socket_address& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if the two objects are different.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const socket_address& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object is less than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const socket_address& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object is greater than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const socket_address& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object is less than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const socket_address& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object is greater than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const socket_address& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hash code for this object.
         */
        size_t hash() const;

    private:
        std::array<char, 128> m_data;
    };


} //namespace netlib


namespace std {


    /**
     * Specialization of std::hash for netlib::socket_address.
     */
    template <> struct hash<netlib::socket_address> {
        /**
         * Returns addr.hash().
         * @param addr object to get the hash of.
         * @return the object's hash.
         */
        size_t operator ()(const netlib::socket_address& addr) const {
            return addr.hash();
        }
    };


}


#endif //NETLIB_SOCKET_ADDRESS_HPP
