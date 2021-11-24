#include "system.hpp"
#include <stdexcept>
#include <algorithm>
#include "netlib/internet_address.hpp"


namespace netlib {


    static_assert(internet_address::data_size >= std::max(sizeof(in_addr), sizeof(in6_addr)));


    //get address from hostname
    static int get_address_from_hostname(const char* address, int af, void* addr) {

        //get address info
        addrinfo* ad;
        int error = getaddrinfo(address, nullptr, nullptr, &ad);

        //throw for error
        if (error) {
            throw std::runtime_error(get_last_error(error));
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
        throw std::runtime_error(std::string("invalid internet address: ") + address);
    }


    //constructor
    internet_address::internet_address(const char* address, int af) {

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
        if (gethostname(buf, sizeof(buf) - 1) == -1) {
            throw std::runtime_error(get_last_error());
        }

        //get address from this host
        m_address_family = get_address_from_hostname(buf, af, m_data);
    }


} //namespace netlib
