#ifndef NETLIB_ENDIANESS_HPP
#define NETLIB_ENDIANESS_HPP


#include <type_traits>
#include <cstring>


/**
 * Network endianess.
 * If true, the network endianess shall be little endian, otherwise network endianess shall be big endian.
 * The default is true, i.e. data are sent to the network with little endian order.
 */
#ifndef NETLIB_NETWORK_ENDIANESS_LITTLE
#define NETLIB_NETWORK_ENDIANESS_LITTLE true
#endif


namespace netlib {


    /**
     * Checks if this platform is little endian.
     * @return true if this platform is little endian, false otherwise.
     */
    inline constexpr bool is_little_endian() noexcept {
        return static_cast<const unsigned char&>(static_cast<const unsigned short>(0x0201)) == 0x01;
    }


    /**
     * Checks if bytes can be copied without endianess swapping,
     * depending on platform and project settings.
     */
    inline constexpr bool can_copy_bytes_without_endianess_swap() {
        return is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE);
    }


    /**
     * Copies the bytes of the given variable (of a trivial type) either in big endian or in little endian format,
     * depending on platform and project settings.
     * @param dst destination buffer.
     * @param src source variable.
     */
    template <class T> std::enable_if_t<std::is_trivial_v<T>> swap_endianess(void* dst, const T& src) {
        if (can_copy_bytes_without_endianess_swap()) {
            std::memcpy(dst, &src, sizeof(T));
        }
        else {
            for (size_t i = 0; i < sizeof(T); ++i) {
                reinterpret_cast<char*>(dst)[sizeof(T) - 1 - i] = reinterpret_cast<const char*>(&src)[i];
            }
        }
    }


    /**
     * Swaps endianess in-place,
     * depending on platform and project settings.
     * @param var variable.
     */
    template <class T> std::enable_if_t<std::is_trivial_v<T>> swap_endianess(T& var) {
        if (!can_copy_bytes_without_endianess_swap()) {
            for (size_t i = 0; i < sizeof(T) / 2; ++i) {
                const char temp = reinterpret_cast<char*>(&var)[i];
                reinterpret_cast<char*>(&var)[i] = reinterpret_cast<char*>(&var)[sizeof(T) - 1 - i];
                reinterpret_cast<char*>(&var)[sizeof(T) - 1 - i] = temp;
            }
        }
    }


} //namespace netlib


#endif //NETLIB_ENDIANESS_HPP
