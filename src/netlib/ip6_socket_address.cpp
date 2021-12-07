#include "netlib/ip6_socket_address.hpp"
#include "combined_hash.hpp"


namespace netlib::ip6 {


    //Returns the hashcode of this address.
    size_t socket_address::hash() const {
        return combined_hash(m_address, m_port_number);
    }


} //namespace netlib::ip6
