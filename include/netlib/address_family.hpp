#ifndef NETLIB_ADDRESS_FAMILY_HPP
#define NETLIB_ADDRESS_FAMILY_HPP


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


} //namespace netlib


#endif //NETLIB_ADDRESS_FAMILY_HPP
