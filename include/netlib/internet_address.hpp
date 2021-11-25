#ifndef NETLIB_INTERNET_ADDRESS_HPP
#define NETLIB_INTERNET_ADDRESS_HPP


#include <functional>
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
        static const internet_address ipv4_any;

        /**
         * Loopback address for ipv4.
         */
        static const internet_address ipv4_loopback;

        /**
         * Any address for ipv4.
         */
        static const internet_address ipv6_any;

        /**
         * Loopback address for ipv6.
         */
        static const internet_address ipv6_loopback;

        /**
         * Non-initializing constructor.
         */
        internet_address() {}

        /**
         * Constructs an internet address.
         * @param address address string: hostname or ipv4/ipv6 address; 
         *  if null or empty, then the address is set to the address of the localhost.
         * @param af address family; if 0, then the appropriate address family is autodetected
         *  from the address string.
         * @exception std::invalid_argument if the address or the address family is invalid.
         * @exception std::runtime_error thrown if the hostname cannot be retrieved.
         */
        internet_address(const char* address, int af = 0);

        /**
         * Constructs an internet address.
         * @param address address string: hostname or ipv4/ipv6 address;
         *  if null or empty, then the address is set to the address of the localhost.
         * @param af address family.
         * @exception std::invalid_argument if the address or the address family is invalid.
         * @exception std::runtime_error thrown if the hostname cannot be retrieved.
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
         * @exception std::runtime_error thrown if the address family is unsupported.
         */
        size_t size() const;

        /**
         * converts the internet address to a string.
         * @exception std::runtime_error thrown if the address family is invalid.
         */
        std::string to_string() const;

        /**
         * Checks if this address matches the given address.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         */
        bool operator == (const internet_address& other) const;

        /**
         * Checks if this address does not match the given address.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         */
        bool operator != (const internet_address& other) const;

        /**
         * Checks if this address comes before the given address.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         * @exception std::invalid_argument thrown if the given address is of different address family.
         */
        bool operator < (const internet_address& other) const;

        /**
         * Checks if this address comes after the given address.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         * @exception std::invalid_argument thrown if the given address is of different address family.
         */
        bool operator > (const internet_address& other) const;

        /**
         * Checks if this address comes before the given address or they are equal.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         * @exception std::invalid_argument thrown if the given address is of different address family.
         */
        bool operator <= (const internet_address& other) const;

        /**
         * Checks if this address comes after the given address or they are equal.
         * The comparison is done on the binary data of the addresses.
         * @param other the other address to compare this to.
         * @return true if the operation succeeded, false otherwise.
         * @exception std::invalid_argument thrown if the given address is of different address family.
         */
        bool operator >= (const internet_address& other) const;

        /**
         * Returns the hash value of this address.
         * @return the hash value of this address.
         */
        size_t hash() const noexcept;

    private:
        int m_address_family;
        char m_data[data_size];

        //internal constructor
        internet_address(const void* data, int af);

        friend class utility;
    };


    /**
     * Converts an internet address to a string.
     * @param addr address.
     * @return string.
     * @exception std::invalid_argument thrown if the conversion to string cannot be done.
     */
    std::string internet_address_to_string(const internet_address& addr);


    /**
     * Converts a string to an internet address.
     * @param str string.
     * @return internet address.
     * @exception std::invalid_argument thrown if the conversion from string cannot be done.
     */
    internet_address internet_address_from_string(const std::string& str);


} //namespace netlib


namespace std {


    /**
     * Get hash value for netlib::internet_address.
     */
    template <> struct hash<netlib::internet_address> {
        /**
         * Returns addr.hash().
         */
        size_t operator ()(const netlib::internet_address& addr) const noexcept {
            return addr.hash();
        }
    };


} //namespace std


#endif //NETLIB_INTERNET_ADDRESS_HPP
