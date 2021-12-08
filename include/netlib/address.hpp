#ifndef NETLIB_ADDRESS_HPP
#define NETLIB_ADDRESS_HPP


#include <variant>
#include "ip4_address.hpp"
#include "ip6_address.hpp"


namespace netlib {


    /**
     * A generic address is either an ip4 address or an ip6 address.
     */
    class address : public std::variant<ip4::address, ip6::address> {
    public:
        /**
         * Variant type.
         */
        using variant_type = std::variant<ip4::address, ip6::address>;

        /**
         * Uses the variant's constructors.
         */
        using variant_type::variant;

        /**
         * Uses the variant's assignment operators.
         */
        using variant_type::operator =;

        /**
         * Converts this address to a string.
         */
        std::string to_string() const;

        /**
         * Compares this object with the given one.
         * @param other the other object to compare this to.
         * @return -1 if this is less than the given object, 0 if they are equal, 1 if this is greater than the given object.
         */
        int compare(const address& other) const;

        /**
         * Checks if this and the given object are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const address& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if this and the given object are diferent.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const address& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object comes before the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const address& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object comes after the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const address& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object comes before the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const address& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object comes after the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const address& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hash value for this object.
         */
        size_t hash() const;
    };


} //namespace netlib


namespace std {
    template <> struct hash<netlib::address> {
        size_t operator ()(const netlib::address& addr) const {
            return addr.hash();
        }
    };
}


#endif //NETLIB_ADDRESS_HPP
