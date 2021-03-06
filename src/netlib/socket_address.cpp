#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/socket_address.hpp"
#include "hash.hpp"


namespace netlib {


    //the default constructor.
    socket_address::socket_address() : m_data{} {
        reinterpret_cast<sockaddr_in*>(m_data.data())->sin_family = AF_UNSPEC;
    }


    //ip4 socket address.
    socket_address::socket_address(const ip_address& addr, uint16_t port) {
        static_assert(sizeof(m_data) >= sizeof(sockaddr_storage));

        switch (addr.address_family()) {
        case AF_INET:
            reinterpret_cast<uint32_t&>(reinterpret_cast<sockaddr_in*>(m_data.data())->sin_addr) = *reinterpret_cast<const uint32_t*>(addr.data());
            reinterpret_cast<sockaddr_in*>(m_data.data())->sin_family = AF_INET;
            reinterpret_cast<sockaddr_in*>(m_data.data())->sin_port = htons(port);
            memset(&reinterpret_cast<sockaddr_in*>(m_data.data())->sin_zero, 0, sizeof(reinterpret_cast<sockaddr_in*>(m_data.data())->sin_zero));
            break;

        case AF_INET6:
            reinterpret_cast<std::array<char, 16>&>(reinterpret_cast<sockaddr_in6*>(m_data.data())->sin6_addr) = *reinterpret_cast<const std::array<char, 16>*>(addr.data());
            reinterpret_cast<sockaddr_in6*>(m_data.data())->sin6_family = AF_INET6;
            reinterpret_cast<sockaddr_in6*>(m_data.data())->sin6_flowinfo = 0;
            reinterpret_cast<sockaddr_in6*>(m_data.data())->sin6_port = htons(port);
            reinterpret_cast<sockaddr_in6*>(m_data.data())->sin6_scope_id = addr.zone_index();
            break;
        }
    }


    //Returns the socket address family.
    int socket_address::address_family() const {
        return reinterpret_cast<const sockaddr*>(m_data.data())->sa_family;
    }


    //Returns the address.
    ip_address socket_address::address() const {
        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            return ip_address(ntohl(reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr)));

        case AF_INET6:
            return ip_address(reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr), reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id);
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns the port.
    uint16_t socket_address::port() const {
        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            return ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port);

        case AF_INET6:
            return ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port);
        }

        throw std::logic_error("Invalid address family.");

    }


    //Converts the address to string.
    std::string socket_address::to_string() const {
        char buffer[INET6_ADDRSTRLEN];

        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            if (inet_ntop(AF_INET, &reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr, buffer, sizeof(buffer))) {
                return std::string(buffer) + ':' + std::to_string(ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port));
            }
            throw std::system_error(get_last_error_number(), std::system_category());

        case AF_INET6:
            if (inet_ntop(AF_INET6, &reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr, buffer, sizeof(buffer))) {
                return '[' + 
                       std::string(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id != 0 ? 
                       std::string(buffer) + '%' + std::to_string(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id) : 
                       std::string(buffer)) + 
                       "]:" + 
                       std::to_string(ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port));
            }
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        throw std::logic_error("Invalid address family.");
    }


    //compare socket addresses
    int socket_address::compare(const socket_address& other) const {
        switch (address_family()) {
        case AF_INET:
            switch (other.address_family()) {
            case AF_INET: 
                return reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) < reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_addr) ? -1 :
                       reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) > reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_addr) ?  1 : 
                       ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port) < ntohs(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_port) ? -1 : 
                       ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port) > ntohs(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_port) ?  1 : 
                       0;

            case AF_INET6:
                return reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) < reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_addr) ? -1 :
                       reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) > reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_addr) ?  1 :
                       ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port) < ntohs(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_port) ? -1 :
                       ntohs(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port) > ntohs(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_port) ? 1 :
                       0;
            }

        case AF_INET6:
            switch (other.address_family()) {
            case AF_INET:
                return reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr) < reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_addr) ? -1 :
                       reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr) > reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_addr) ?  1 :
                       ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port) < ntohs(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_port) ? -1 :
                       ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port) > ntohs(reinterpret_cast<const sockaddr_in*>(other.m_data.data())->sin_port) ? 1 :
                       0;

            case AF_INET6:
                return reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr) < reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_addr) ? -1 :
                       reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr) > reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_addr) ?  1 : 
                       reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id < reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_scope_id ? -1 :
                       reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id > reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_scope_id ?  1 :
                       ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port) < ntohs(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_port) ? -1 :
                       ntohs(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port) > ntohs(reinterpret_cast<const sockaddr_in6*>(other.m_data.data())->sin6_port) ? 1 :
                       0;
            }
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns the hash code for this object.
    size_t socket_address::hash() const {
        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            return netlib::hash(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr, reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_port);

        case AF_INET6:
            return netlib::hash(reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr, reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_port, reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_scope_id);
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns true if the address of this is any.
    bool socket_address::is_any() const {
        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            return reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) == htonl(INADDR_ANY);

        case AF_INET6:
            return memcmp(&reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr, &in6addr_any, sizeof(in6addr_any)) == 0;
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns true if the address of this is loopback.
    bool socket_address::is_loopback() const {
        switch (reinterpret_cast<const sockaddr*>(m_data.data())->sa_family) {
        case AF_INET:
            return reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(m_data.data())->sin_addr) == htonl(INADDR_LOOPBACK);

        case AF_INET6:
            return memcmp(&reinterpret_cast<const sockaddr_in6*>(m_data.data())->sin6_addr, &in6addr_loopback, sizeof(in6addr_loopback)) == 0;
        }

        throw std::logic_error("Invalid address family.");
    }


} //namespace netlib

