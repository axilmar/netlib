#ifndef NETLIB_IP_ADDRESS_HPP
#define NETLIB_IP_ADDRESS_HPP


#include <cstddef>
#include <string>
#include "constants.hpp"
#include "byte.hpp"


namespace netlib {


    /**
     * IP address.
     */
    class ip_address : public constants {
    public:
        /** 
         * buffer size.
         */ 
        static constexpr size_t BUFFER_SIZE = 16;

        /**
         * any address tag. 
         */
        struct any_address_type {};

        /**
         * loopback address tag.
         */
        struct loopback_address_type {};

        /**
         * ip4 any address.
         */
        static const ip_address ip4_any_address;

        /**
         * ip4 loopback address.
         */
        static const ip_address ip4_loopback_address;

        /**
         * ip6 any address.
         */
        static const ip_address ip6_any_address;

        /**
         * ip6 loopback address.
         */
        static const ip_address ip6_loopback_address;

        /**
         * Returns the host name of this machine.
         */
        static std::string get_host_name();

        /**
         * Creates an empty, non-valid network address.
         */
        ip_address() {}

        /**
         * Creates an empty network address of the given address family.
         * @param af address family.
         */
        ip_address(int af);

        /**
         * Creates a network address from a string.
         * @param addr_string IP4/IP6 address string, or hostname; if null/empty, then the hostname is auto-discovered.
         * @param address_family address family.
         * @exception socket_error thrown if the hostname could not be resolved.
         */
        ip_address(const char* addr_string, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Creates a network address from a string.
         * @param addr_string IP4/IP6 address string, or hostname; if null/empty, then the hostname is auto-discovered.
         * @param address_family address family.
         * @exception socket_error thrown if the hostname could not be resolved.
         */
        ip_address(std::string addr_string, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Creates a network address for any address.
         * @param any_address constant.
         * @param address_family address family.
         * @exception socket_error thrown if any of the parameters are not supported.
         */
        ip_address(any_address_type address, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Creates a network address for the loopback address.
         * @param any_address constant.
         * @param address_family address family.
         * @exception socket_error thrown if any of the parameters are not supported.
         */
        ip_address(loopback_address_type address, int address_family = constants::ADDRESS_FAMILY_IP4);

        /**
         * Converts the address to a string.
         * @return string that represents this address.
         * @exception socket_error thrown if the address is not valid.
         */
        std::string to_string() const;

        /**
         * Returns pointer to internal buffer. 
         */
        const byte* data() const { return m_data; }

        /**
         * Returns pointer to internal buffer.
         */
        byte* data() { return m_data; }

        /**
         * Returns size of internal data. 
         */
        constexpr size_t size() const { return BUFFER_SIZE; }

        /**
         * Returns the address family. 
         */
        int get_address_family() const { return m_address_family; }

        /**
         * Returns the ip6 flow info. 
         */
        uint32_t get_ip6_flow_info() const { return m_ip6_flow_info; }

        /**
         * Returns the ip6 zone id. 
         */
        uint32_t get_ip6_zone_id() const { return m_ip6_zone_id; }

    private:
        //data
        int m_address_family{};
        byte m_data[BUFFER_SIZE]{};
        uint32_t m_ip6_flow_info{};
        uint32_t m_ip6_zone_id{};

        friend class library;
        friend class socket_address;
    };


} //namespace netlib


#endif //NETLIB_IP_ADDRESS_HPP
