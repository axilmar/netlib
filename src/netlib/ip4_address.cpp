#include "platform.hpp"
#include <cstring>
#include <stdexcept>
#include "netlib/ip4_address.hpp"


namespace netlib::ip4 {


    //constants
    const address address::any = INADDR_ANY;
    const address address::loopback = INADDR_LOOPBACK;


    //set address from hostname/ip address
    void address::set(const char* hostname) {
        if (!hostname || strlen(hostname) == 0) {
            char hostname[256];
            if (gethostname(hostname, sizeof(hostname))) {
                throw std::runtime_error(get_last_error_message());
            }

            bool found = get_address_info(hostname, [&](addrinfo* ai) {
                if (ai->ai_family == AF_INET) {
                    m_value = reinterpret_cast<sockaddr_in*>(ai->ai_addr)->sin_addr.S_un.S_addr;
                    return true;
                }
                return false;
                });

            if (!found) {
                throw std::runtime_error("The ip4 address of the localhost cannot be retrieved.");
            }

            return;
        }

        if (inet_pton(AF_INET, hostname, &m_value) == 1) {
            return;
        }

        bool found = get_address_info(hostname, [&](addrinfo* ai) {
            if (ai->ai_family == AF_INET) {
                m_value = reinterpret_cast<sockaddr_in*>(ai->ai_addr)->sin_addr.S_un.S_addr;
                return true;
            }
            return false;
            });

        if (!found) {
            throw std::invalid_argument("Hostname string is not a valid hostname/ip4 address.");
        }
    }


    std::string address::to_string() const {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &m_value, buf, sizeof(buf));
        return buf;
    }


} //namespace netlib::ip4
