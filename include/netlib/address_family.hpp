#ifndef NETLIB_ADDRESS_FAMILY_HPP
#define NETLIB_ADDRESS_FAMILY_HPP


#include <string>


namespace netlib {


    /**
     * Address family.
     */
    enum class address_family {
        /**
         * Internet protocol version 4.
         */
        ipv4,

        /**
         * Internet protocol version 6.
         */
        ipv6
    };


    /**
     * Converts address family current system address family value.
     * @param af address family enum value.
     * @return system value for address family.
     * @exception std::invalid_argument thrown if the given address family is invalid.
     */
    int address_family_to_system_value(address_family af);


    /**
     * Converts system value to address family.
     * @param af address family system value.
     * @return address value enumeration.
     * @exception std::invalid_argument thrown if the given value is not supported.
     */
    address_family system_value_to_address_family(int af);


    /**
     * Converts the address family to a string.
     * @param af address family.
     * @return string.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    std::string address_family_to_string(address_family af);


    /**
     * Converts a string to an address family.
     * @param str string.
     * @return address family.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    address_family address_family_from_string(const std::string& str);


} //namespace netlib


#endif //NETLIB_ADDRESS_FAMILY_HPP
