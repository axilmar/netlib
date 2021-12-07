#ifndef NETLIB_COMBINED_HASH_HPP
#define NETLIB_COMBINED_HASH_HPP


#include <functional>
#include <string_view>


namespace netlib {


    //layout bytes of variables into a flat array,
    //then compute its hash as a string view.


    template <class H> void fill_array(char* a, const H& h) {
        memcpy(a, &h, sizeof(H));
    }


    template <class H, class... T> void fill_array(char* a, const H& h, const T&... t) {
        memcpy(a, &h, sizeof(H));
        fill_array(a + sizeof(H), t...);
    }


    template <class H, class... T> size_t combined_hash(const H& h, const T&... t) {
        static constexpr size_t sz = (sizeof(H) + ... + sizeof(T));
        char a[sz];
        fill_array(a, h, t...);
        return std::hash<std::string_view>()({ a, sz });
    }


} //namespace netlib


#endif //NETLIB_COMBINED_HASH_HPP
