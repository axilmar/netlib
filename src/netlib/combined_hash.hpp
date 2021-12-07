#ifndef NETLIB_COMBINED_HASH_HPP
#define NETLIB_COMBINED_HASH_HPP


#include <functional>
#include <string_view>


namespace netlib {


    //layout bytes of variables into a flat array,
    //then compute its hash as a string view.


    #pragma pack(push, 1)


    template <class H, class ...T> struct hash_byte_array : hash_byte_array<T...> {
        char v[sizeof(H)];
        hash_byte_array(const H& h, const T&... t) : hash_byte_array<T...>(t...) {
            memcpy(v, &h, sizeof(H));
        }
    };


    template <class H> struct hash_byte_array<H> {
        char v[sizeof(H)];
        hash_byte_array(const H& h) {
            memcpy(v, &h, sizeof(H));
        }
    };


    #pragma pack(pop)


    template <class...Values> size_t combined_hash(const Values&... values) {
        const hash_byte_array<Values...> a(values...);        
        return std::hash<std::string_view>()({ reinterpret_cast<const char*>(&a), sizeof(a) });
    }


} //namespace netlib


#endif //NETLIB_COMBINED_HASH_HPP
