#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/ip_address.hpp"
#include "hash.hpp"


namespace netlib {


    //constants
    const int ip_address::ip4 = AF_INET;
    const int ip_address::ip6 = AF_INET6;


    //standard addresses
    const ip_address ip4_any(INADDR_ANY);
    const ip_address ip4_loopback(INADDR_LOOPBACK);
    const ip_address ip6_any(reinterpret_cast<const std::array<char, 16>&>(in6addr_any));
    const ip_address ip6_loopback(reinterpret_cast<const std::array<char, 16>&>(in6addr_loopback));


    //the default constructor.
    ip_address::ip_address()
        : m_address_type{AF_INET}
    {
        reinterpret_cast<uint32_t&>(m_data) = 0;
    }


    //ip4 constructor.
    ip_address::ip_address(uint32_t addr) 
        : m_address_type(AF_INET)
    {
        reinterpret_cast<uint32_t&>(m_data) = addr;
    }


    //ip4 constructor.
    ip_address::ip_address(const std::array<char, 4>& addr) 
        : m_address_type(AF_INET)
    {
        reinterpret_cast<std::array<char, 4>&>(m_data) = addr;
    }


    //ip6 constructor
    ip_address::ip_address(const std::array<char, 16>& addr, uint32_t zone_index)
        : m_address_type(AF_INET6)
        , m_zone_index(zone_index)
    {
        reinterpret_cast<std::array<char, 16>&>(m_data) = addr;
    }


    //ip6 constructor
    ip_address::ip_address(const std::array<uint16_t, 8>& addr, uint32_t zone_index)
        : m_address_type(AF_INET6)
        , m_zone_index(zone_index)
    {
        reinterpret_cast<std::array<uint16_t, 8>&>(m_data) = addr;
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
            throw std::invalid_argument("Invalid address type.");
        }

        //if hostname is given, try to convert string to ip address
        if (hostname && strlen(hostname) > 0) {
            if (type == AF_INET || !type) {
                if (inet_pton(AF_INET, hostname, m_data.data()) == 1) {
                    m_address_type = AF_INET;
                    return;
                }
            }

            if (type == AF_INET6 || !type) {
                const char* zone_index_str = strchr(hostname, '%');
                if (!zone_index_str) {
                    if (inet_pton(AF_INET6, hostname, m_data.data()) == 1) {
                        m_address_type = AF_INET6;
                        m_zone_index = 0;
                        return;
                    }
                }

                else {
                    char* p;
                    long v = strtol(zone_index_str + 1, &p, 10);
                    if (!*p && v >= 0 && inet_pton(AF_INET6, hostname, m_data.data()) == 1) {
                        m_address_type = AF_INET6;
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
                throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
            }
            hostname = localhost_name;
        }

        //get address info
        addrinfo* ai;
        int error = getaddrinfo(hostname, nullptr, nullptr, &ai);
        if (error) {
            throw std::system_error(error, std::system_category(), get_error_message(error));
        }

        addrinfo* fai = nullptr;

        //autodetect address type
        if (type == 0) {
            for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
                if (tai->ai_family == AF_INET || tai->ai_family == AF_INET6) {
                    fai = tai;
                    break;
                }
            }
        }

        //else find specific address type
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
                m_address_type = AF_INET;
                reinterpret_cast<uint32_t&>(m_data) = reinterpret_cast<const uint32_t&>(reinterpret_cast<sockaddr_in*>(fai->ai_addr)->sin_addr);
                break;

            case AF_INET6:
                m_address_type = AF_INET6;
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


    //compare ip addresses
    int ip_address::compare(const ip_address& other) const {
        int r = memcmp(m_data.data(), other.m_data.data(), m_address_type == AF_INET ? 4 : 16);
        return r ? r : m_zone_index < other.m_zone_index ? -1 : m_zone_index > other.m_zone_index ? 1 : 0;
    }


    //hash
    size_t ip_address::hash() const {
        return netlib::hash(m_address_type, m_data, m_zone_index);
    }


} //namespace netlib
