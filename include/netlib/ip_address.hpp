#ifndef NETLIB_IP_ADDRESS_HPP
#define NETLIB_IP_ADDRESS_HPP


#include <cstdint>
#include <array>
#include <string>
#include <functional>


namespace netlib {


    /**
     * IP address.
     */
    class ip_address {
    public:
        /**
         * Internet prococol 4 address type.
         */
        static const int IP4;

        /**
         * Internet prococol 6 address type.
         */
        static const int IP6;

        /**
         * Any address for ip4.
         */
        static const ip_address ip4_any;

        /**
         * loopback address for ip4.
         */
        static const ip_address ip4_loopback;

        /**
         * Any address for ip6.
         */
        static const ip_address ip6_any;

        /**
         * loopback address for ip6.
         */
        static const ip_address ip6_loopback;

        /**
         * The default constructor.
         * The address contains all zeros, and the address type is ip4.
         */
        ip_address();

        /**
         * Constructor from ip4 address.
         * @param addr address (in network byte order).
         */
        ip_address(uint32_t addr);

        /**
         * Constructor from ip4 address.
         * @param addr address.
         */
        ip_address(const std::array<char, 4>& addr);

        /**
         * Constructor from ip6 address.
         * @param addr address.
         */
        ip_address(const std::array<char, 16>& addr, uint32_t zone_index = 0);

        /**
         * Constructor from ip6 address.
         * @param addr address.
         */
        ip_address(const std::array<uint16_t, 8>& addr, uint32_t zone_index = 0);

        /**
         * Constructor from hostname/ip address string.
         * @param hostname hostname/ip address string; 
         *  if null/empty, then the ip address of the localhost is discovered.
         * @param type address type; if 0, then it is autodetected.
         * @exception std::invalid_argument if the address type is not supported.
         * @exception std::system_error if a system error happens.
         * @exception std::logic_error if the hostname is valid but cannot be resolved to an IP4/IP6 address.
         */
        ip_address(const char* hostname, int type = 0);

        /**
         * Constructor from hostname/ip address string.
         * @param hostname hostname/ip address string;
         *  if null/empty, then the ip address of the localhost is discovered.
         * @param type address type; if 0, then it is autodetected.
         */
        ip_address(const std::string& hostname, int type = 0) 
            : ip_address(hostname.c_str(), type)
        {
        }

        /**
         * Returns the address type.
         */
        int type() const {
            return m_address_type;
        }

        /**
         * Returns the address data.
         */
        const char* data() const {
            return m_data.data();
        }

        /**
         * Returns the zone index.
         */
        uint32_t zone_index() const {
            return m_zone_index;
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
        int compare(const ip_address& other) const;

        /**
         * Checks if the two objects are equal.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const ip_address& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if the two objects are different.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const ip_address& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object is less than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const ip_address& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object is greater than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const ip_address& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object is less than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const ip_address& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object is greater than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const ip_address& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hash code for this object.
         */
        size_t hash() const;

    private:
        int m_address_type;
        std::array<char, 16> m_data;
        uint32_t m_zone_index;
    };


} //namespace netlib


namespace std {


    /**
     * Specialization of std::hash for netlib::ip_address. 
     */
    template <> struct hash<netlib::ip_address> {
        /**
         * Returns addr.hash().
         * @param addr object to get the hash of.
         * @return the object's hash.
         */
        size_t operator ()(const netlib::ip_address& addr) const {
            return addr.hash();
        }
    };


}


#endif //NETLIB_IP_ADDRESS_HPP
