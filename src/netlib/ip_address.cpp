#include "netlib/ip_address.hpp"


namespace netlib {


    //ip4 any address.
    const ip_address ip_address::ip4_any_address{ ip_address::any_address_type{}, ip_address::ADDRESS_FAMILY_IP4 };


    //ip4 loopback address.
    const ip_address ip_address::ip4_loopback_address{ ip_address::loopback_address_type{}, ip_address::ADDRESS_FAMILY_IP4 };


    //ip6 any address.
    const ip_address ip_address::ip6_any_address{ ip_address::any_address_type{}, ip_address::ADDRESS_FAMILY_IP6 };


    //ip6 loopback address.
    const ip_address ip_address::ip6_loopback_address{ ip_address::loopback_address_type{}, ip_address::ADDRESS_FAMILY_IP6 };


} //namespace netlib
