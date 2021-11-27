#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include "internet_address.hpp"


namespace netlib {


    /**
     * A socket address. 
     */
    class socket_address {
    public:
        /**
         * size of internal data. 
         */
        static constexpr size_t data_size = 128;

        /**
         * Non-initializing constructor. 
         */
        socket_address() {}

        /**
         * Constructor from address and port.
         * @param address address.
         * @param port port.
         * @exception std::invalid_argument thrown if the address family is invalid.
         */
        socket_address(const internet_address& address, uint16_t port);

        /**
         * Returns the data. 
         */
        const void* data() const { return m_data; }

        /**
         * Returns the data.
         */
        void* data() { return m_data; }

        /**
         * Returns the size of the underlying structure,
         * based on its address family.
         * @exception std::runtime_error thrown if the address family is invalid.
         */
        size_t size() const;

        /**
         * Returns the address family. 
         */
        int address_family() const;

        /**
         * Returns the address. 
         * @exception std::runtime_error thrown if the address family is invalid.
         */
        internet_address address() const;

        /**
         * Returns the port. 
         * @exception std::runtime_error thrown if the address family is invalid.
         */
        uint16_t port() const;

        /**
         * Converts the socket address to a string.
         * @return a string with the following form: <internet address>:<port>. 
         */
        std::string to_string() const;

        /**
         * Checks if this socket address is equal to the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         */
        bool operator == (const socket_address& other) const;

        /**
         * Checks if this socket address is different than the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         */
        bool operator != (const socket_address& other) const;

        /**
         * Checks if this socket address is less than the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         * @exception std::invalid_argument thrown if the given socket address is of another address family.
         */
        bool operator < (const socket_address& other) const;

        /**
         * Checks if this socket address is greater than the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         * @exception std::invalid_argument thrown if the given socket address is of another address family.
         */
        bool operator > (const socket_address& other) const;

        /**
         * Checks if this socket address is less than or equal to the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         * @exception std::invalid_argument thrown if the given socket address is of another address family.
         */
        bool operator <= (const socket_address& other) const;

        /**
         * Checks if this socket address is greater than or equal to the given socket address.
         * @param other the other socket address.
         * @return true on success, false on failure.
         * @exception std::invalid_argument thrown if the given socket address is of another address family.
         */
        bool operator >= (const socket_address& other) const;

        /**
         * Returns the hash value for this socket address. 
         */
        size_t hash() const noexcept;

    private:
        char m_data[data_size];
    };


} //namespace netlib


namespace std {


    /**
     * Get hash value for netlib::socket_address.
     */
    template <> struct hash<netlib::socket_address> {
        /**
         * Returns addr.hash().
         */
        size_t operator ()(const netlib::socket_address& addr) const noexcept {
            return addr.hash();
        }
    };


} //namespace std


#endif //NETLIB_SOCKET_ADDRESS_HPP
