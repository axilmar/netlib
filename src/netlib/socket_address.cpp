#include "netlib/platform.hpp"
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include "netlib/socket_address.hpp"


namespace netlib {


    static_assert(socket_address::data_size >= sizeof(sockaddr_storage));


    //constructor.
    socket_address::socket_address(const internet_address& address, uint16_t port) 
        : m_data{}
    {
        switch (address.address_family()) {
        case AF_INET:
            std::memcpy(&reinterpret_cast<sockaddr_in*>(m_data)->sin_addr, address.data(), sizeof(in_addr));
            reinterpret_cast<sockaddr_in*>(m_data)->sin_family = AF_INET;
            reinterpret_cast<sockaddr_in*>(m_data)->sin_port = htons(port);
            break;

        case AF_INET6:
            std::memcpy(&reinterpret_cast<sockaddr_in6*>(m_data)->sin6_addr, address.data(), sizeof(in6_addr));
            reinterpret_cast<sockaddr_in6*>(m_data)->sin6_family = AF_INET6;
            reinterpret_cast<sockaddr_in6*>(m_data)->sin6_port = htons(port);
            reinterpret_cast<sockaddr_in6*>(m_data)->sin6_scope_id = address.m_ipv6_scope_id;
            break;

        default:
            throw std::invalid_argument("Invalid address family.");
        }
    }


    //Returns the size of the underlying structure,
    size_t socket_address::size() const {
        switch (reinterpret_cast<const sockaddr*>(m_data)->sa_family) {
        case AF_INET:
            return sizeof(sockaddr_in);

        case AF_INET6:
            return sizeof(sockaddr_in6);
        }

        throw std::runtime_error("Invalid address family.");
    }


    //Returns the address family.
    int socket_address::address_family() const {
        return reinterpret_cast<const sockaddr*>(m_data)->sa_family;
    }

    //Returns the address.
    internet_address socket_address::address() const {
        switch (reinterpret_cast<const sockaddr*>(m_data)->sa_family) {
        case AF_INET:
            return internet_address(&reinterpret_cast<const sockaddr_in*>(m_data)->sin_addr, AF_INET);

        case AF_INET6:
            return internet_address(&reinterpret_cast<const sockaddr_in6*>(m_data)->sin6_addr, AF_INET6, reinterpret_cast<const sockaddr_in6*>(m_data)->sin6_scope_id);
        }

        throw std::runtime_error("Invalid address family.");

    }

    //Returns the port.
    uint16_t socket_address::port() const {
        switch (reinterpret_cast<const sockaddr*>(m_data)->sa_family) {
        case AF_INET:
            return ntohs(reinterpret_cast<const sockaddr_in*>(m_data)->sin_port);

        case AF_INET6:
            return ntohs(reinterpret_cast<const sockaddr_in6*>(m_data)->sin6_port);
        }

        throw std::runtime_error("Invalid address family.");
    }


    //Converts the socket address to a string.
    std::string socket_address::to_string() const {
        return address().to_string() + ':' + std::to_string(port());
    }




    //Checks if this socket address is equal to the given socket address.
    bool socket_address::operator == (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) == 0;
        }

        return memcmp(m_data, other.m_data, data_size) == 0;
    }


    //Checks if this socket address is different than the given socket address.
    bool socket_address::operator != (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) != 0;
        }

        return memcmp(m_data, other.m_data, data_size) != 0;
    }


    //Checks if this socket address is less than the given socket address.
    bool socket_address::operator < (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family != reinterpret_cast<const sockaddr*>(other.m_data)->sa_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) < 0;
        }

        return memcmp(m_data, other.m_data, data_size) < 0;
    }


    //Checks if this socket address is greater than the given socket address.
    bool socket_address::operator > (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family != reinterpret_cast<const sockaddr*>(other.m_data)->sa_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) > 0;
        }

        return memcmp(m_data, other.m_data, data_size) > 0;
    }


    //Checks if this socket address is less than or equal to the given socket address.
    bool socket_address::operator <= (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family != reinterpret_cast<const sockaddr*>(other.m_data)->sa_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) <= 0;
        }

        return memcmp(m_data, other.m_data, data_size) <= 0;
    }


    //Checks if this socket address is greater than or equal to the given socket address.
    bool socket_address::operator >= (const socket_address& other) const {
        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family != reinterpret_cast<const sockaddr*>(other.m_data)->sa_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(sockaddr_in)) >= 0;
        }

        return memcmp(m_data, other.m_data, data_size) >= 0;
    }


    //get hash value
    size_t socket_address::hash() const noexcept {
        return std::hash<std::string_view>()(std::string_view(m_data, reinterpret_cast<const sockaddr*>(m_data)->sa_family == AF_INET ? sizeof(sockaddr_in) : data_size));
    }


} //namespace netlib