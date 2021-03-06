#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/ip_address.hpp"
#include "hash.hpp"


namespace netlib {


    //ip4 family address constant
    ip_address::ip4::operator int() const {
        return AF_INET;
    }


    //ip4 constant addresses
    const struct ip_address::ip4 ip_address::ip4;
    const ip_address ip_address::ip4::any(static_cast<uint32_t>(INADDR_ANY));
    const ip_address ip_address::ip4::loopback(INADDR_LOOPBACK);


    //ip6 family address constant
    ip_address::ip6::operator int() const {
        return AF_INET6;
    }


    //ip6 constant addresses
    const struct ip_address::ip6 ip_address::ip6;
    const ip_address ip_address::ip6::any(reinterpret_cast<const std::array<char, 16>&>(in6addr_any));
    const ip_address ip_address::ip6::loopback(reinterpret_cast<const std::array<char, 16>&>(in6addr_loopback));


    //the default constructor.
    ip_address::ip_address()
        : m_address_family{AF_UNSPEC}
    {
        reinterpret_cast<uint32_t&>(m_data) = 0;
    }


    //ip4 constructor.
    ip_address::ip_address(uint32_t addr) 
        : m_address_family(AF_INET)
    {
        reinterpret_cast<uint32_t&>(m_data) = htonl(addr);
    }


    //ip4 constructor.
    ip_address::ip_address(const std::array<char, 4>& addr) 
        : m_address_family(AF_INET)
    {
        reinterpret_cast<std::array<char, 4>&>(m_data) = addr;
    }


    //ip6 constructor
    ip_address::ip_address(const std::array<char, 16>& addr, uint32_t zone_index)
        : m_address_family(AF_INET6)
        , m_zone_index(zone_index)
    {
        reinterpret_cast<std::array<char, 16>&>(m_data) = addr;
    }


    //ip6 constructor
    ip_address::ip_address(const std::array<uint16_t, 8>& addr, uint32_t zone_index)
        : m_address_family(AF_INET6)
        , m_zone_index(zone_index)
    {
        for (size_t i = 0; i < 8; ++i) {
            reinterpret_cast<std::array<uint16_t, 8>&>(m_data)[i] = htons(addr[i]);
        }
    }


    //constructor from hostname/ip address
    ip_address::ip_address(const char* hostname, int type) {
        char localhost_name[HOST_NAME_MAX + 1];

        //check type
        switch (type) {
        case 0:
        case AF_INET:
        case AF_INET6:
            break;
        default:
            throw std::invalid_argument("Invalid address family.");
        }

        //if hostname is given, try to convert string to ip address
        if (hostname && strlen(hostname) > 0) {
            if (type == AF_INET || !type) {
                if (inet_pton(AF_INET, hostname, m_data.data()) == 1) {
                    m_address_family = AF_INET;
                    return;
                }
            }

            if (type == AF_INET6 || !type) {
                const char* zone_index_str = strchr(hostname, '%');
                if (!zone_index_str) {
                    if (inet_pton(AF_INET6, hostname, m_data.data()) == 1) {
                        m_address_family = AF_INET6;
                        m_zone_index = 0;
                        return;
                    }
                }

                else {
                    char* p;
                    long v = strtol(zone_index_str + 1, &p, 10);
                    if (!*p && v >= 0 && inet_pton(AF_INET6, hostname, m_data.data()) == 1) {
                        m_address_family = AF_INET6;
                        m_zone_index = v;
                        return;
                    }
                }
            }
        }

        //else if hostname is not given, find the name of the localhost
        else {
            //get localhost name
            localhost_name[HOST_NAME_MAX] = '\0';
            if (gethostname(localhost_name, sizeof(localhost_name))) {
                throw std::system_error(get_last_error_number(), std::system_category());
            }
            hostname = localhost_name;
        }

        //get address info
        addrinfo* ai;
        int error = getaddrinfo(hostname, nullptr, nullptr, &ai);
        if (error) {
            throw std::system_error(error, std::system_category());
        }

        addrinfo* fai = nullptr;

        //autodetect address family
        if (type == 0) {
            for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
                if (tai->ai_family == AF_INET || tai->ai_family == AF_INET6) {
                    fai = tai;
                    break;
                }
            }
        }

        //else find specific address family
        else {
            for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
                if (tai->ai_family == type) {
                    fai = tai;
                    break;
                }
            }
        }

        //if address found
        if (fai) {
            switch (fai->ai_family) {
            case AF_INET:
                m_address_family = AF_INET;
                reinterpret_cast<uint32_t&>(m_data) = reinterpret_cast<const uint32_t&>(reinterpret_cast<sockaddr_in*>(fai->ai_addr)->sin_addr);
                break;

            case AF_INET6:
                m_address_family = AF_INET6;
                m_data = reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<sockaddr_in6*>(fai->ai_addr)->sin6_addr);
                m_zone_index = reinterpret_cast<sockaddr_in6*>(fai->ai_addr)->sin6_scope_id;
                break;
            }
        }

        //free the memory allocated from getaddrinfo
        freeaddrinfo(ai);

        //if address not found
        if (!fai) {
            throw std::logic_error("IP4/IP6 address not found for hostname = " + std::string(hostname));
        }

        static_assert(sizeof(m_data) >= sizeof(in6_addr));
    }


    /**
     * Returns the ip4 value.
     */
    uint32_t ip_address::ip4_value() const {
        return ntohl(reinterpret_cast<const uint32_t&>(m_data));
    }


    //Returns the words of the ip6 address.
    std::array<uint16_t, 8> ip_address::ip6_words() const {
        std::array<uint16_t, 8> r;

        for (size_t i = 0; i < 8; ++i) {
            r[i] = ntohs(reinterpret_cast<const std::array<uint16_t, 8>&>(m_data)[i]);
        }

        return r;
    }


    //Converts the address to string.
    std::string ip_address::to_string() const {
        char buffer[256];

        switch (m_address_family) {
        case AF_INET:
            return inet_ntop(AF_INET, m_data.data(), buffer, sizeof(buffer));

        case AF_INET6: {
            std::string result = inet_ntop(AF_INET6, m_data.data(), buffer, sizeof(buffer));
            return m_zone_index == 0 ? result : result + '%' + std::to_string(m_zone_index);
        }
        }

        throw std::logic_error("Invalid address family.");
    }


    //compare ip addresses
    int ip_address::compare(const ip_address& other) const {
        int r = memcmp(m_data.data(), other.m_data.data(), m_address_family == AF_INET ? 4 : 16);
        return r ? r : m_zone_index < other.m_zone_index ? -1 : m_zone_index > other.m_zone_index ? 1 : 0;
    }


    //hash
    size_t ip_address::hash() const {
        return netlib::hash(m_address_family, m_data, m_zone_index);
    }


    //Returns true if the address of this is any.
    bool ip_address::is_any() const {
        switch (m_address_family) {
        case AF_INET:
            return *reinterpret_cast<const uint32_t*>(m_data.data()) == htonl(INADDR_ANY);

        case AF_INET6:
            return memcmp(m_data.data(), &in6addr_any, sizeof(in6addr_any)) == 0;
        }

        throw std::logic_error("Invalid address family.");
    }


    //Returns true if the address of this is loopback.
    bool ip_address::is_loopback() const {
        switch (m_address_family) {
        case AF_INET:
            return *reinterpret_cast<const uint32_t*>(m_data.data()) == htonl(INADDR_LOOPBACK);

        case AF_INET6:
            return memcmp(m_data.data(), &in6addr_loopback, sizeof(in6addr_loopback)) == 0;
        }

        throw std::logic_error("Invalid address family.");
    }


} //namespace netlib
