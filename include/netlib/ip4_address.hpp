#ifndef NETLIB_IP4_ADDRESS_HPP
#define NETLIB_IP4_ADDRESS_HPP


#include <cassert>
#include <cstdint>
#include <array>
#include <string>


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
         * byte array type. 
         */
        using bytes_type = std::array<uint8_t, 4>;

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
        address(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) : m_bytes{ b0, b1, b2, b3 } {
        }

        /**
         * Constructor from hostname/ip address string.
         * @param hostname hostname/ip address. It can be:
         *  - a null or empty string; the ip address of the localhost is discovered and stored in this object.
         *  - a hostname; the ip address of the given host is discovered and stored in this object.
         *  - an ip address string.
         * @exception std::runtime_error thrown if the given string is not a valid hostname/ip address.
         * @exception std::runtime_error thrown if the localhost's name or ip address could not be retrieved.
         */
        address(const char* hostname) : m_value{} {
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
         * @exception std::runtime_error thrown if the given string is not a valid hostname/ip address.
         * @exception std::runtime_error thrown if the localhost's name or ip address could not be retrieved.
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
         * Converts the address to string, in the form of XXX.XXX.XXX.XXX.
         * @return a string representation of this address.
         */
        std::string to_string() const;

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


#endif //NETLIB_IP4_ADDRESS_HPP
