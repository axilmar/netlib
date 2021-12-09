#ifndef NETLIB_ENDIANESS_HPP
#define NETLIB_ENDIANESS_HPP


#include <cstdint>
#include <type_traits>


/**
 * Preprocessor definition for network endianess.
 * Data are sent with big endianess over the network, if true.
 */
#ifndef NETLIB_NETWORK_ENDIANESS_IS_BIG
#define NETLIB_NETWORK_ENDIANESS_IS_BIG false
#endif


namespace netlib {


    /**
     * big endian flag.
     */
    inline constexpr bool big_endian = static_cast<const uint8_t&>(uint16_t(0x0a01)) == 0x0a;


    /**
     * Sets endianess for given variable depending on NETLIB_NETWORK_ENDIANESS_IS_BIG and if big_endian is true.
     */
    template <class T> std::enable_if_t<std::is_trivial_v<T>> set_endianess(T& var) {
        if constexpr (NETLIB_NETWORK_ENDIANESS_IS_BIG != big_endian) {
            for (size_t i = 0; i < sizeof(T) / 2; ++i) {
                const char temp = reinterpret_cast<char*>(&var)[i];
                reinterpret_cast<char*>(&var)[i] = reinterpret_cast<char*>(&var)[sizeof(T) - 1 - i];
                reinterpret_cast<char*>(&var)[sizeof(T) - 1 - i] = temp;
            }
        }
    }


    /**
     * Sets endianess for given variable depending on NETLIB_NETWORK_ENDIANESS_IS_BIG and if big_endian is true.
     * The variable is copied in another variable.
     */
    template <class T> std::enable_if_t<std::is_trivial_v<T>> set_endianess(const T& var, T& out) {
        if constexpr (NETLIB_NETWORK_ENDIANESS_IS_BIG != big_endian) {
            for (size_t i = 0; i < sizeof(T); ++i) {
                reinterpret_cast<char*>(&out)[i] = reinterpret_cast<const char*>(&var)[sizeof(T) - 1 - i];
            }
        }
        else {
            out = var;
        }
    }


} //namespace netlib


#endif //NETLIB_ENDIANESS_HPP
