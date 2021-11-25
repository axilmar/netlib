#include "system.hpp"
#include <stdexcept>
#include <algorithm>
#include "netlib/internet_address.hpp"


namespace netlib {


    static_assert(internet_address::data_size >= std::max(sizeof(in_addr), sizeof(in6_addr)));


    //constant addresses
    static const auto inaddr_any      = ntohl(INADDR_ANY     );
    static const auto inaddr_loopback = ntohl(INADDR_LOOPBACK);
    const internet_address internet_address::ipv4_any     (&inaddr_any      , AF_INET );
    const internet_address internet_address::ipv4_loopback(&inaddr_loopback , AF_INET );
    const internet_address internet_address::ipv6_any     (&in6addr_any     , AF_INET6);
    const internet_address internet_address::ipv6_loopback(&in6addr_loopback, AF_INET6);


    //get address from hostname
    static int get_address_from_hostname(const char* address, int af, void* addr) {

        //get address info
        addrinfo* ad;
        int error = getaddrinfo(address, nullptr, nullptr, &ad);

        //throw for error
        if (error) {
            throw std::invalid_argument(get_last_error(error));
        }

        //find address to return
        for (; ad; ad = ad->ai_next) {
            //skip the current entry if the address family is not requested
            if (af && ad->ai_family != af) {
                continue;
            }

            //get ipv4 address
            if (af == AF_INET) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN*>(ad->ai_addr)->sin_addr, sizeof(in_addr));
                return AF_INET;
            }

            //get ipv6 address
            if (af == AF_INET6) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN6*>(ad->ai_addr)->sin6_addr, sizeof(in6_addr));
                return AF_INET6;
            }
        }

        //invalid address
        throw std::invalid_argument(std::string("invalid internet address: ") + address);
    }


    //constructor
    internet_address::internet_address(const char* address, int af) {

        //check address family
        switch (af) {
        case 0:
        case AF_INET:
        case AF_INET6:
            break;

        default:
            throw std::invalid_argument("Invalid address family value: " + std::to_string(af));
        }

        //if address is given
        if (address && strlen(address) > 0) {

            //try conversion to specific address family
            if (inet_pton(af, address, m_data) == 1) {
                m_address_family = af;
                return;
            }

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
        else if (af == AF_INET6) {
            memcpy(m_data, data, sizeof(in6_addr));
        }
        else {
            throw std::invalid_argument("unsupported address family");
        }
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
