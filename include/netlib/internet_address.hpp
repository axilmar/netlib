#ifndef NETLIB_INTERNET_ADDRESS_HPP
#define NETLIB_INTERNET_ADDRESS_HPP


#include "address_family.hpp"


namespace netlib {


    /**
     * Internet address.
     */
    class internet_address {
    public:
        /**
         * Size, in bytes, of the internal buffer.
         */
        static constexpr size_t data_size = 16;

        /**
         * Any address for ipv4.
         */
        static const internet_address ipv4_any_address;

        /**
         * Loopback address for ipv4.
         */
        static const internet_address ipv4_loopback_address;

        /**
         * Any address for ipv4.
         */
        static const internet_address ipv6_any_address;

        /**
         * Loopback address for ipv6.
         */
        static const internet_address ipv6_loopback_address;

        /**
         * Non-initializing constructor.
         */
        internet_address() {}

        /**
         * Constructs an internet address.
         * @param address address string: hostname or ipv4/ipv6 address; 
         *  if null or empty or invalid, then the address is set to the address of the localhost.
         * @param af address family; if 0, then the appropriate address family is autodetected
         *  from the address string.
         * @exception std::runtime_error if the address and address family does not represent an internet address.
         */
        internet_address(const char* address, int af = 0);

        /**
         * Constructs an internet address.
         * @param address address string: hostname or ipv4/ipv6 address;
         *  if null or empty or invalid, then the address is set to the address of the localhost.
         * @param af address family.
         * @exception std::runtime_error if the address and address family does not represent an internet address.
         */
        internet_address(const char* address, address_family af) 
            : internet_address(address, address_family_to_system_value(af)) {}

        /**
         * Returns the address family.
         */
        int address_family() const noexcept { return m_address_family; }

        /**
         * Returns the data.
         */
        const void* data() const noexcept { return m_data; }

        /**
         * Returns the data.
         */
        void* data() noexcept { return m_data; }

        /**
         * Returns the size of the data.
         */
        constexpr size_t size() const noexcept { return data_size; }

    private:
        int m_address_family;
        char m_data[data_size];
    };


} //namespace netlib


#endif //NETLIB_INTERNET_ADDRESS_HPP
