#include "netlib/ip4_socket_address.hpp"
#include "combined_hash.hpp"


namespace netlib::ip4 {


    //comparison
    int socket_address::compare(const socket_address& other) const {
        //compare ips
        int r = m_address.compare(other.m_address);

        //if ips are different, return that
        if (r) {
            return r;
        }

        //ips are the same, compare port numbers
        return m_port_number < other.m_port_number ? -1 : m_port_number > other.m_port_number ? 1 : 0;
    }


    //Returns the hashcode of this address.
    size_t socket_address::hash() const {
        return combined_hash(m_address, m_port_number);
    }


} //namespace netlib::ip4
