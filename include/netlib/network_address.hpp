#ifndef NETLIB_NETWORK_ADDRESS_HPP
#define NETLIB_NETWORK_ADDRESS_HPP


#include <cstddef>
#include <string>
#include "constants.hpp"
#include "byte.hpp"


namespace netlib {


    /**
     * Network address.
     */
    class network_address : public constants {
    public:
        /**
         * Returns the host name of this machine.
         */
        static std::string get_host_name();

        /**
         * Creates an empty, non-valid network address.
         */
        network_address() {}

        /**
         * Creates an empty network address of the given address family.
         * @param af address family.
         */
        network_address(int af);

        /**
         * Creates a network address from a string.
         * @param addr_string IP4/IP6 address string, or hostname; if null/empty, then the hostname is auto-discovered.
         * @param address_family address family.
         * @exception socket_error thrown if the hostname could not be resolved.
         */
        network_address(const char* addr_string, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Creates a network address from a string.
         * @param addr_string IP4/IP6 address string, or hostname; if null/empty, then the hostname is auto-discovered.
         * @param address_family address family.
         * @exception socket_error thrown if the hostname could not be resolved.
         */
        network_address(std::string addr_string, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Converts the address to a string.
         * @return string that represents this address.
         * @exception socket_error thrown if the address is not valid.
         */
        std::string to_string() const;

    private:
        //buffer size
        static constexpr size_t BUFFER_SIZE = 16;

        //data
        int m_address_family{};
        byte m_data[BUFFER_SIZE]{};
        uint32_t m_ip6_flow_info{};
        uint32_t m_ip6_zone_id{};

        friend class library;
        friend class socket_address;
    };


} //namespace netlib


#endif //NETLIB_NETWORK_ADDRESS_HPP
