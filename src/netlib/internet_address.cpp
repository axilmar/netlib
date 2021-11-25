#include "netlib/platform.hpp"
#include <stdexcept>
#include <algorithm>
#include <string_view>
#include "netlib/internet_address.hpp"


namespace netlib {


    static_assert(internet_address::data_size >= std::max(sizeof(in_addr), sizeof(in6_addr)));


    //constant addresses
    static const auto inaddr_any = ntohl(INADDR_ANY);
    static const auto inaddr_loopback = ntohl(INADDR_LOOPBACK);
    const internet_address internet_address::ipv4_any(&inaddr_any, AF_INET);
    const internet_address internet_address::ipv4_loopback(&inaddr_loopback, AF_INET);
    const internet_address internet_address::ipv6_any(&in6addr_any, AF_INET6);
    const internet_address internet_address::ipv6_loopback(&in6addr_loopback, AF_INET6);


    //get address from hostname
    static int get_address_from_hostname(const char* address, int af, void* addr) {

        //get address info
        addrinfo* ai;
        int error = getaddrinfo(address, nullptr, nullptr, &ai);

        //throw for error
        if (error) {
            throw std::invalid_argument(get_last_error(error));
        }

        addrinfo* start_ai = ai;

        //find address to return
        for (; ai; ai = ai->ai_next) {
            //skip the current entry if the address family is not requested
            if (af && ai->ai_family != af) {
                continue;
            }

            //get ipv4 address
            if (af == AF_INET) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN*>(ai->ai_addr)->sin_addr, sizeof(in_addr));
                freeaddrinfo(start_ai);
                return AF_INET;
            }

            //get ipv6 address
            if (af == AF_INET6) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN6*>(ai->ai_addr)->sin6_addr, sizeof(in6_addr));
                freeaddrinfo(start_ai);
                return AF_INET6;
            }
        }

        freeaddrinfo(start_ai);

        //invalid address
        throw std::invalid_argument(std::string("invalid internet address: ") + address);
    }


    //constructor
    internet_address::internet_address(const char* address, int af) 
        : m_data{}
    {

        //if address is given
        if (address && strlen(address) > 0) {

            //try conversion to specific address family
            if (inet_pton(af, address, m_data) == 1) {
                m_address_family = af;
                return;
            }

            //autodetect
            if (af == 0) {
                //discover ipv4 address
                if (inet_pton(AF_INET, address, m_data) == 1) {
                    m_address_family = AF_INET;
                    return;
                }

                //discover ipv6 address
                if (inet_pton(AF_INET6, address, m_data) == 1) {
                    m_address_family = AF_INET6;
                    return;
                }
            }

            //get address from hostname
            m_address_family = get_address_from_hostname(address, af, m_data);
            return;
        }

        //the address is not given, get hostname and get its address

        //buffer for hostname; 
        //in case of truncation, the returned string is not-null terminated,
        //so provide one extra slot for null-termination.
        char buf[257];
        buf[256] = '\0';

        //get the host name
        const int result = gethostname(buf, sizeof(buf) - 1);
        if (result == -1) {
            throw std::runtime_error(get_last_error());
        }

        //get address from this host
        m_address_family = get_address_from_hostname(buf, af, m_data);
    }


    //internal constructor
    internet_address::internet_address(const void* data, int af)
        : m_address_family(af)
    {
        if (af == AF_INET) {
            memcpy(m_data, data, sizeof(in_addr));
        }

        memcpy(m_data, data, data_size);
    }


    //Returns the size of the data.
    size_t internet_address::size() const {
        switch (m_address_family) {
        case AF_INET:
            return sizeof(in_addr);

        case AF_INET6:
            return sizeof(in6_addr);
        }

        throw std::runtime_error("Unsupported address family");
    }


    //converts the internet address to a string.
    std::string internet_address::to_string() const {
        char buf[256];

        const char* str = inet_ntop(m_address_family, m_data, buf, sizeof(buf));

        if (str) {
            return str;
        }

        throw std::runtime_error(get_last_error());
    }


    //Checks if this address matches the given address.
    bool internet_address::operator == (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            return false;
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) == 0;
        }

        return memcmp(m_data, other.m_data, data_size) == 0;
    }


    //Checks if this address matches the given address.
    bool internet_address::operator != (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            return true;
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) != 0;
        }

        return memcmp(m_data, other.m_data, data_size) != 0;
    }


    //Checks if this address comes before the given address.
    bool internet_address::operator < (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) < 0;
        }

        return memcmp(m_data, other.m_data, data_size) < 0;
    }


    //Checks if this address comes after the given address.
    bool internet_address::operator > (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) > 0;
        }

        return memcmp(m_data, other.m_data, data_size) > 0;
    }


    //Checks if this address comes before the given address or they are equal.
    bool internet_address::operator <= (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) <= 0;
        }

        return memcmp(m_data, other.m_data, data_size) <= 0;
    }


    //Checks if this address comes after the given address or they are equal.
    bool internet_address::operator >= (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) >= 0;
        }

        return memcmp(m_data, other.m_data, data_size) >= 0;
    }


    //get hash value
    size_t internet_address::hash() const noexcept {
        return std::hash<std::string_view>()(std::string_view(m_data, m_address_family == AF_INET ? sizeof(in_addr) : data_size));
    }


    //Converts an internet address to a string.
    std::string internet_address_to_string(const internet_address& addr) {
        try {
            return addr.to_string();
        }
        catch (const std::runtime_error& ex) {
            throw std::invalid_argument(ex.what());
        }
    }


    //Converts a string to an internet address.
    internet_address internet_address_from_string(const std::string& str) {
        try {
            return { str.c_str() };
        }
        catch (const std::runtime_error& ex) {
            throw std::invalid_argument(ex.what());
        }
    }


} //namespace netlib
