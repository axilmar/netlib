#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include <variant>
#include "ip4_socket_address.hpp"
#include "ip6_socket_address.hpp"
#include "address.hpp"


namespace netlib {


    /**
     * A generic socket address is either an ip4 address or an ip6 address.
     */
    class socket_address : public std::variant<ip4::socket_address, ip6::socket_address> {
    public:
        /**
         * Variant type.
         */
        using variant_type = std::variant<ip4::socket_address, ip6::socket_address>;

        /**
         * The default constructor.
         */
        socket_address() {
        }

        /**
         * Constructor from ip4 address and port number.
         * @param addr address.
         * @param port port number.
         */
        socket_address(const ip4::address& addr, netlib::port_number port = 0)
            : variant_type(ip4::socket_address(addr, port))
        {
        }

        /**
         * Constructor from ip6 address and port number.
         * @param addr address.
         * @param port port number.
         */
        socket_address(const ip6::address& addr, netlib::port_number port = 0)
            : variant_type(ip6::socket_address(addr, port))
        {
        }

        /**
         * Uses the variant's constructors.
         */
        using variant_type::variant;

        /**
         * Uses the variant's assignment operators.
         */
        using variant_type::operator =;

        /**
         * Returns the current address.
         * @return the current address.
         */
        netlib::address address() const;

        /**
         * Sets this socket address to be an ip4 address.
         * @param addr address.
         */
        void set_address(const ip4::address& addr);

        /**
         * Sets this socket address to be an ip6 address.
         * @param addr address.
         */
        void set_address(const ip6::address& addr);

        /**
         * Returns the port number.
         * @return the port number.
         */
        netlib::port_number port_number() const;

        /**
         * Sets the port number.
         * @param port port number.
         */
        void set_port_number(netlib::port_number port);

        /**
         * Converts this address to a string.
         */
        std::string to_string() const;

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
         * Returns the hash value for this object.
         */
        size_t hash() const;
    };


} //namespace netlib


namespace std {
    template <> struct hash<netlib::socket_address> {
        size_t operator ()(const netlib::socket_address& addr) const {
            return addr.hash();
        }
    };
}


#endif //NETLIB_SOCKET_ADDRESS_HPP
