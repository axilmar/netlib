#include "netlib/platform.hpp"
#include <stdexcept>
#include "netlib/utility.hpp"


namespace netlib {


    //private access
    class utility {
    public:
        static internet_address make_internet_address(void* addr, int af) {
            return { addr, af };
        }
    };


    //retrieves the hostname
    std::string get_host_name() {
        char buf[257];
        buf[256] = '\0';

        const int result = gethostname(buf, sizeof(buf) - 1);

        if (result == 0) {
            return buf;
        }

        throw std::runtime_error(get_last_error());
    }


    //returns the internet addresses of the given host.
    std::vector<internet_address> get_addresses(const char* addr) {
        addrinfo* ai;
        int error = getaddrinfo(addr, nullptr, nullptr, &ai);

        if (error) {
            throw std::invalid_argument(get_last_error(error));
        }

        addrinfo* start_ai = ai;
        std::vector<internet_address> result;

        for (; ai; ai = ai->ai_next) {
            sockaddr* sa = reinterpret_cast<sockaddr*>(ai->ai_addr);

            switch (sa->sa_family) {
                case AF_INET:
                    result.push_back(utility::make_internet_address(&reinterpret_cast<SOCKADDR_IN*>(sa)->sin_addr, AF_INET));
                    break;

                case AF_INET6:
                    result.push_back(utility::make_internet_address(&reinterpret_cast<SOCKADDR_IN6*>(sa)->sin6_addr, AF_INET6));
                    break;
            }
        }

        freeaddrinfo(start_ai);

        return result;
    }


} //namespace netlib
