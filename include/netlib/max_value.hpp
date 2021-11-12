#ifndef NETLIB_MAX_VALUE_HPP
#define NETLIB_MAX_VALUE_HPP


#include <cstdint>
#include <limits>


namespace netlib {


    namespace internals {


        template <uint64_t V> auto get_max_uint() {
            if constexpr (V <= std::numeric_limits<uint8_t>::max()) return uint8_t{};
            else if (V <= std::numeric_limits<uint16_t>::max()) return uint16_t{};
            else if (V <= std::numeric_limits<uint32_t>::max()) return uint32_t{};
            else return uint64_t{};
        }


    } //namespace internals


    /**
     * Max uint type, based on value.
     */
    template <uint64_t Value> 
    using max_uint_t = decltype(internals::get_max_uint<Value>());


} //namespace netlib


#endif //NETLIB_MAX_VALUE_HPP
