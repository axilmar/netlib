#include "system.hpp"
#include <stdexcept>
#include "netlib/address_family.hpp"


namespace netlib {


    //convert address family to system value
    int address_family_to_system_value(address_family af) {
        switch (af) {
        case address_family::ipv4:
            return AF_INET;

        case address_family::ipv6:
            return AF_INET6;
        }

        throw std::invalid_argument("Invalid address family.");
    }


    //convert system value to address family
    address_family system_value_to_address_family(int af) {
        switch (af) {
        case AF_INET:
            return address_family::ipv4;

        case AF_INET6:
            return address_family::ipv6;
        }

        throw std::invalid_argument("Unsupported system address family value.");
    }


    //Converts the address family to a string.
    std::string address_family_to_string(address_family af) {
        switch (af) {
        case address_family::ipv4:
            return "ipv4";

        case address_family::ipv6:
            return "ipv6";
        }

        throw std::invalid_argument("Invalid address family.");
    }


    //Converts a string to an address family.
    address_family address_family_from_string(const std::string& str) {
        if (str == "ipv4") {
            return address_family::ipv4;
        }

        if (str == "ipv6") {
            return address_family::ipv6;
        }

        throw std::invalid_argument("Unsupported address family value.");
    }


} //namespace netlib
