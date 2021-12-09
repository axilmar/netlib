#ifndef NETLIB_HASH_HPP
#define NETLIB_HASH_HPP


#include <type_traits>
#include <string_view>
#include <array>


namespace netlib {


    template <class H> void fill_hash_buffer(char* buffer, const H& h) {
        static_assert(std::is_trivial_v<H>);
        memcpy(buffer, &h, sizeof(h));
    }


    template <class H, class... T> void fill_hash_buffer(char* buffer, const H& h, const T&... t) {
        fill_hash_buffer(buffer, h);
        fill_hash_buffer(buffer + sizeof(h), t...);
    }


    /**
     * Compute hash of multiple values.
     * @param v values.
     * @return hash of multiple values.
     */
    template <class... T> size_t hash(const T&... v) {
        static constexpr size_t size = (sizeof(v) + ...);
        std::array<char, size> buffer;
        fill_hash_buffer(buffer.data(), v...);
        return std::hash<std::string_view>()({buffer.data(), size});
    }


} //namespace netlib


#endif //NETLIB_HASH_HPP
