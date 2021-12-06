#ifndef NETLIB_IP6_ADDRESS_HPP
#define NETLIB_IP6_ADDRESS_HPP


#include <cstdint>
#include <array>
#include <string>


namespace netlib::ip6 {


    /**
     * Internet protocol 6 address.
     */
    class address {
    public:
        /**
         * byte array type.
         */
        using bytes_type = std::array<uint8_t, 16>;

        /**
         * word array type.
         */
        using words_type = std::array<uint16_t, 8>;

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
        address() : m_words{}, m_zone_index{} {
        }

        /**
         * Constructor from words array.
         * @param words words.
         * @param zone_index zone index.
         */
        address(const words_type& words, uint32_t zone_index = 0) 
            : m_words(words), m_zone_index(zone_index)
        {
        }

        /**
         * Constructor from byte array.
         * @param bytes bytes.
         * @param zone_index zone index.
         */
        address(const bytes_type& bytes, uint32_t zone_index = 0) 
            : m_bytes{ bytes }, m_zone_index(zone_index)
        {
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
         * Assignment from words.
         * @param words words.
         */
        address& operator = (const words_type& words) {
            m_words = words;
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
         * Returns the words.
         * @return the words.
         */
        const words_type& words() const {
            return m_words;
        }

        /**
         * Sets the words and the zone index.
         * @param words words.
         * @param zone_index zone index.
         */
        void set(const words_type& words, uint32_t zone_index = 0) {
            m_words = words;
            m_zone_index = zone_index;
        }

        /**
         * Returns the bytes.
         * @return the bytes.
         */
        const bytes_type& bytes() const {
            return m_bytes;
        }

        /**
         * Sets the bytes and the zone index.
         * @param bytes bytes.
         * @param zone_index zone index.
         */
        void set(const bytes_type& bytes, uint32_t zone_index = 0) {
            m_bytes = bytes;
            m_zone_index = zone_index;
        }

        /**
         * Returns the zone index.
         * @return the zone index.
         */
        uint32_t zone_index() const {
            return m_zone_index;
        }

        /**
         * Sets the zone index.
         * @param zone_index zone index.
         */
        void set_zone_index(uint32_t zone_index) {
            m_zone_index = zone_index;
        }

        /**
         * Converts the address to string.
         * @return a string representation of this address.
         */
        std::string to_string() const;

    private:
        //data
        union {
            words_type m_words;
            bytes_type m_bytes;
        };
        uint32_t m_zone_index;

        //sets the address from hostname/ip6 address.
        void set(const char* hostname);
    };


} //namespace netlib::ip6


#endif //NETLIB_IP6_ADDRESS_HPP
