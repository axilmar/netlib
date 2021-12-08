#ifndef NETLIB_IP4_ADDRESS_HPP
#define NETLIB_IP4_ADDRESS_HPP


#include <cstdint>
#include <array>
#include <string>
#include <functional>


namespace netlib::ip4 {


    /**
     * Internet protocol 4 address. 
     */
    class address {
    public:
        /**
         * value type.
         */
        using value_type = uint32_t;

        /**
         * byte type. 
         */
        using byte_type = uint8_t;

        /**
         * byte array type. 
         */
        using bytes_type = std::array<byte_type, 4>;

        /**
         * Any address. 
         */
        static const address any;

        /**
         * The loopback address.
         */
        static const address loopback;

        /**
         * The default constructor.
         * It zeroes all bytes.
         */
        address() : m_value{} {
        }

        /**
         * Constructor from value.
         * @param value value.
         */
        address(value_type value) : m_value(value) {
        }

        /**
         * Constructor from byte array. 
         * @param bytes bytes.
         */
        address(const bytes_type& bytes) : m_bytes{bytes} {
        }

        /**
         * Constructor from individual bytes.
         * @param b0 byte at position 0.
         * @param b1 byte at position 1.
         * @param b2 byte at position 2.
         * @param b3 byte at position 3.
         */
        address(byte_type b0, byte_type b1, byte_type b2, byte_type b3) : m_bytes{ b0, b1, b2, b3 } {
        }

        /**
         * Constructor from hostname/ip address string.
         * @param hostname hostname/ip address. It can be:
         *  - a null or empty string; the ip address of the localhost is discovered and stored in this object.
         *  - a hostname; the ip address of the given host is discovered and stored in this object.
         *  - an ip address string.
         * @exception std::runtime_error thrown 
         *  if the given string is not a valid hostname/ip address or
         *  if the localhost's name or ip address could not be retrieved.
         */
        address(const char* hostname) : address() {
            set(hostname);
        }

        /**
         * @copydoc address(const char*). 
         */
        address(const std::string& hostname) : address(hostname.c_str()) {
        }

        /**
         * Assignment from value.
         * @param value value, in network byte order.
         */
        address& operator = (value_type value) {
            m_value = value;
            return *this;
        }

        /**
         * Assignment from bytes.
         * @param bytes bytes.
         * @return reference to this.
         */
        address& operator = (const bytes_type& bytes) {
            m_bytes = bytes;
            return *this;
        }

        /**
         * Assignment from hostname/ip address. 
         * Same as address(const char*).
         * @exception std::runtime_error thrown
         *  if the given string is not a valid hostname/ip address or
         *  if the localhost's name or ip address could not be retrieved.
         */
        address& operator = (const char* hostname) {
            set(hostname);
            return *this;
        }

        /**
         * @copydoc operator = (const char*).
         */
        address& operator = (const std::string& hostname) {
            return operator = (hostname.c_str());
        }

        /**
         * Returns the value of this address.
         * @return the value of this address.
         */
        value_type value() const {
            return m_value;
        }

        /**
         * Returns the bytes.
         * @return the bytes.
         */
        const bytes_type bytes() const {
            return m_bytes;
        }

        /**
         * Converts the address to string.
         * @return a string representation of this address.
         */
        std::string to_string() const;

        /**
         * Compares this object with the given one.
         * @param other the other object to compare this to.
         * @return -1 if this is less than the given object, 0 if they are equal, 1 if this is greater than the given object.
         */
        int compare(const address& other) const {
            return m_value < other.m_value ? -1 : m_value > other.m_value ? 1 : 0;
        }

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
         * Returns the hashcode of this address.
         */
        size_t hash() const {
            return std::hash<value_type>()(m_value);
        }

    private:
        //data
        union {
            value_type m_value;
            bytes_type m_bytes;
        };

        //set from hostname/ip4 address.
        void set(const char* hostname);
    };


} //namespace netlib::ip4


namespace std {
    template <> struct hash<netlib::ip4::address> {
        size_t operator ()(const netlib::ip4::address& addr) const {
            return addr.hash();
        }
    };
}


#endif //NETLIB_IP4_ADDRESS_HPP
