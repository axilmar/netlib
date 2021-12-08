#include "platform.hpp"
#include <cstring>
#include <stdexcept>
#include "netlib/ip6_address.hpp"
#include "combined_hash.hpp"


namespace netlib::ip6 {


    ///////////////////////////////////////////////////////////////////////////
    // INTERNALS
    ///////////////////////////////////////////////////////////////////////////


    static_assert(sizeof(address::bytes_type) == sizeof(in6_addr));


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //constants
    const address address::any{ reinterpret_cast<const address::bytes_type&>(in6addr_any) };
    const address address::loopback{ reinterpret_cast<const address::bytes_type&>(in6addr_loopback) };


    //Compares this object with the given one.
    int address::compare(const address& other) const {
        int r = memcmp(m_bytes.data(), other.m_bytes.data(), sizeof(m_bytes));

        if (r) {
            return r;
        }

        return m_zone_index < other.m_zone_index ? -1 : m_zone_index > other.m_zone_index ? 1 : 0;
    }


    //Returns the hashcode of this address.
    size_t address::hash() const {
        return combined_hash(m_words, m_zone_index);
    }


    ///////////////////////////////////////////////////////////////////////////
    // PRIVATE
    ///////////////////////////////////////////////////////////////////////////


    //set address from hostname/ip address
    void address::set(const char* hostname) {
        if (!hostname || strlen(hostname) == 0) {
            char hostname[257]{};
            if (gethostname(hostname, sizeof(hostname))) {
                throw std::runtime_error(get_last_error_message());
            }

            bool found = get_address_info(hostname, [&](addrinfo* ai) {
                if (ai->ai_family == AF_INET6) {
                    m_bytes = reinterpret_cast<const address::bytes_type&>(reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_addr);
                    m_zone_index = reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_scope_id;
                    return true;
                }
                return false;
                });

            if (!found) {
                throw std::runtime_error("The ip6 address of the localhost cannot be retrieved.");
            }

            return;
        }

        const char* zone_index_pos = strchr(hostname, '%');

        //if the address string does not contain a zone index
        if (!zone_index_pos) {
            if (inet_pton(AF_INET6, hostname, m_bytes.data()) == 1) {
                return;
            }
        }

        //else the address string contains a zone index
        else {
            //convert the zone index string to a value
            char* endptr;
            long l = strtol(zone_index_pos + 1, &endptr, 10);

            //if the remainder of string was successfully parsed to an integer,
            //then parse the ip part into an in6_addr
            if (!*endptr && l >= 0) {
                const std::string ip_part(hostname, zone_index_pos);

                //if the ip part is parseable as an ip6 address, then set the found zone id
                if (inet_pton(AF_INET6, ip_part.c_str(), m_bytes.data()) == 1) {
                    m_zone_index = l;
                    return;
                }
            }
        }

        bool found = get_address_info(hostname, [&](addrinfo* ai) {
            if (ai->ai_family == AF_INET6) {
                m_bytes = reinterpret_cast<const address::bytes_type&>(reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_addr);
                m_zone_index = reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_scope_id;
                return true;
            }
            return false;
            });

        if (!found) {
            throw std::runtime_error("Hostname string is not a valid hostname/ip6 address.");
        }
    }


    std::string address::to_string() const {
        char buf[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &m_bytes, buf, sizeof(buf));
        return m_zone_index == 0 ? std::string(buf) : (std::string(buf) + '%' + std::to_string(m_zone_index));
    }


} //namespace netlib::ip6
