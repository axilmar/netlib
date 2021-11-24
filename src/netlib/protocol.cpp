#include "system.hpp"
#include <stdexcept>
#include "netlib/protocol.hpp"


namespace netlib {


    //Converts protocol to system value.
    int protocol_to_system_value(protocol p) {
        switch (p) {
        case protocol::tcp:
            return IPPROTO_TCP;

        case protocol::udp:
            return IPPROTO_UDP;
        }

        throw std::invalid_argument("Invalid protocol.");
    }


    //Converts system value to protocol.
    protocol system_value_to_protocol(int p) {
        switch (p) {
        case IPPROTO_TCP:
            return protocol::tcp;

        case IPPROTO_UDP:
            return protocol::udp;
        }

        throw std::invalid_argument("Unsupported system protocol value.");
    }


} //namespace netlib
