#include <stdexcept>
#include "netlib/socket_address.hpp"


namespace netlib {


    //Returns the current address.
    netlib::address socket_address::address() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).address();

        case 1:
            return std::get<1>(*this).address();
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Sets this socket address to be an ip4 address.
    void socket_address::set_address(const ip4::address& addr) {
        switch (variant_type::index()) {
        case 0:
            std::get<0>(*this).set_address(addr);
            return;

        case 1:
            variant_type::operator = (ip4::socket_address(addr, std::get<1>(*this).port_number()));
            return;
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Sets this socket address to be an ip6 address.
    void socket_address::set_address(const ip6::address& addr) {
        switch (variant_type::index()) {
        case 0:
            variant_type::operator = (ip6::socket_address(addr, std::get<0>(*this).port_number()));
            return;

        case 1:
            std::get<1>(*this).set_address(addr);
            return;
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Returns the port number.
    netlib::port_number socket_address::port_number() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).port_number();

        case 1:
            return std::get<1>(*this).port_number();
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Sets the port number.
    void socket_address::set_port_number(netlib::port_number port) {
        switch (variant_type::index()) {
        case 0:
            std::get<0>(*this).set_port_number(port);
            return;

        case 1:
            std::get<1>(*this).set_port_number(port);
            return;
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Converts this address to a string.
    std::string socket_address::to_string() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).to_string();

        case 1:
            return std::get<1>(*this).to_string();
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //compares socket addresses
    int socket_address::compare(const socket_address& other) const {
        int r;

        switch (variant_type::index()) {
        case 0:
            switch (other.index()) {
            case 0:
                return std::get<0>(*this).compare(std::get<0>(other));
            case 1:
                r = netlib::address(std::get<0>(*this).address()).compare(netlib::address(std::get<1>(other).address()));
                if (r) {
                    return r;
                }
                return std::get<0>(*this).port_number() < std::get<1>(other).port_number() ? -1 : std::get<0>(*this).port_number() > std::get<1>(other).port_number() ? 1 : 0;
            }

        case 1:
            switch (other.index()) {
            case 0:
                r = netlib::address(std::get<1>(*this).address()).compare(netlib::address(std::get<0>(other).address()));
                if (r) {
                    return r;
                }
                return std::get<1>(*this).port_number() < std::get<0>(other).port_number() ? -1 : std::get<1>(*this).port_number() > std::get<0>(other).port_number() ? 1 : 0;
            case 1:
                return std::get<1>(*this).compare(std::get<1>(other));
            }
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Returns the hash value for this object.
    size_t socket_address::hash() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).hash();

        case 1:
            return std::get<1>(*this).hash();
        }

        throw std::runtime_error("Invalid variant index.");
    }


} //namespace netlib
