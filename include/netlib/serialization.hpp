#ifndef NETLIB_SERIALIZATION_HPP
#define NETLIB_SERIALIZATION_HPP


#include <type_traits>
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * The default serialize function for type T.
     * @param value value to serialize.
     * @param buffer output buffer.
     */
    template <class T> void serialize(const T& value, byte_buffer& buffer) {
        static_assert(false, "serialize function not defined for type T");
    }


    /**
     * The default deserialize function for type T.
     * @param value value to deserialize.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     */
    template <class T> void deserialize(T& value, const byte_buffer& buffer, byte_buffer::position& pos) {
        static_assert(false, "deserialize function not defined for type T");
    }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_HPP
