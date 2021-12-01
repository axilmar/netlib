#include "netlib/platform.hpp"
#include <stdexcept>
#include "netlib/utility.hpp"


namespace netlib {


    //private access
    class utility {
    public:
        static internet_address make_internet_address(void* addr, int af, uint32_t ipv6_scope_id = 0) {
            return { addr, af, ipv6_scope_id };
        }
    };


    //retrieves the hostname
    std::string get_host_name() {
        char buf[257];
        buf[256] = '\0';

        //get hostname in order to use it in getaddrinfo
        int error = gethostname(buf, sizeof(buf) - 1);

        //cannot get hostname; throw exception
        if (error != 0) {
            throw std::runtime_error(get_last_error_message());
        }

        //hints for canonical name
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_CANONNAME;

        //use 'getaddrinfo' to get the canonical name
        addrinfo* ai;
        error = getaddrinfo(buf, nullptr, &hints, &ai);

        //cannot get address info; throw exception
        if (error) {
            throw std::invalid_argument(get_last_error_message(error));
        }

        //get result
        std::string result = ai->ai_canonname;

        //address info no longer needed
        freeaddrinfo(ai);

        return result;
    }


    //returns the internet addresses of the given host.
    std::vector<internet_address> get_addresses(const char* addr, int af) {
        addrinfo* ai;
        int error = getaddrinfo(addr, nullptr, nullptr, &ai);

        if (error) {
            throw std::invalid_argument(get_last_error_message(error));
        }

        addrinfo* start_ai = ai;
        std::vector<internet_address> result;

        for (; ai; ai = ai->ai_next) {
            sockaddr* sa = reinterpret_cast<sockaddr*>(ai->ai_addr);

            if ((af == 0 || af == AF_INET) && sa->sa_family == AF_INET) {
                result.push_back(utility::make_internet_address(&reinterpret_cast<SOCKADDR_IN*>(sa)->sin_addr, AF_INET));
            }

            if ((af == 0 || af == AF_INET6) && sa->sa_family == AF_INET6) {
                result.push_back(utility::make_internet_address(&reinterpret_cast<SOCKADDR_IN6*>(sa)->sin6_addr, AF_INET6, reinterpret_cast<SOCKADDR_IN6*>(sa)->sin6_scope_id));
            }
        }

        freeaddrinfo(start_ai);

        return result;
    }


} //namespace netlib
