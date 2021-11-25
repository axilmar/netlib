#include "system.hpp"
#include <stdexcept>
#include "netlib/socket_type.hpp"


namespace netlib {


    //Converts socket type to the system value.
    int socket_type_to_system_value(socket_type st) {
        switch (st) {
        case socket_type::stream:
            return SOCK_STREAM;

        case socket_type::datagram:
            return SOCK_DGRAM;
        }

        throw std::invalid_argument("Invalid socket type.");
    }


    //Converts system value to socket type.
    socket_type system_value_to_socket_type(int st) {
        switch (st) {
        case SOCK_STREAM:
            return socket_type::stream;

        case SOCK_DGRAM:
            return socket_type::datagram;
        }

        throw std::invalid_argument("Unsupported system socket type value.");
    }


    //Converts the socket type to a string.
    std::string socket_type_to_string(socket_type st) {
        switch (st) {
        case socket_type::stream:
            return "stream";

        case socket_type::datagram:
            return "datagram";
        }

        throw std::invalid_argument("Invalid socket type.");
    }


    //Converts a string to an socket type.
    socket_type socket_type_from_string(const std::string& str) {
        if (str == "stream") {
            return socket_type::stream;
        }

        if (str == "datagram") {
            return socket_type::datagram;
        }

        throw std::invalid_argument("Unsupported socket type value.");
    }


} //namespace netlib
