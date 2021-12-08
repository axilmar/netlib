#include <stdexcept>
#include "netlib/address.hpp"


namespace netlib {


    //Converts this address to a string.
    std::string address::to_string() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).to_string();

        case 1:
            return std::get<1>(*this).to_string();
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //comparison
    int address::compare(const address& other) const {
        int r;

        switch (variant_type::index()) {
        case 0:
            switch (other.index()) {
            case 0:
                return std::get<0>(*this).compare(std::get<0>(other));
            case 1:
                r = memcmp(std::get<0>(*this).bytes().data(), std::get<1>(other).bytes().data(), sizeof(ip4::address::bytes_type));
                if (r <= 0) {
                    return -1;
                }
                return 1;
            }

        case 1:
            switch (other.index()) {
            case 0:
                r = memcmp(std::get<1>(*this).bytes().data(), std::get<0>(other).bytes().data(), sizeof(ip4::address::bytes_type));
                if (r < 0) {
                    return -1;
                }
                return 1;
            case 1:
                return std::get<1>(*this).compare(std::get<1>(other));
            }
        }

        throw std::runtime_error("Invalid variant index.");
    }


    //Returns the hash value for this object.
    size_t address::hash() const {
        switch (variant_type::index()) {
        case 0:
            return std::get<0>(*this).hash();

        case 1:
            return std::get<1>(*this).hash();
        }

        throw std::runtime_error("Invalid variant index.");
    }


} //namespace netlib
