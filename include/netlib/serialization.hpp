#ifndef NETLIB_SERIALIZATION_HPP
#define NETLIB_SERIALIZATION_HPP


#include <stdexcept>
#include "byte_buffer.hpp"
#include "container_traits.hpp"
#include "tuple.hpp"


namespace netlib {


    /**
     * Checks if the current platform is little or big endian.
     * @return true if the current platform is little endian, false otherwise.
     */
    inline bool is_little_endian() {
        static const union { uint16_t v16; uint8_t v8[2]; } u{ 0x0201 };
        return u.v8[0] == 0x01;
    }


    /**
     * Copies value to buffer, with endianess.
     * @param buffer destination buffer; must already have size.
     * @param value value to write to buffer.
     */
    template <class T> void copy_value(void* buffer, const T& value) {
        //copy with little endian; little endian is the default
        if (is_little_endian()) {
            *reinterpret_cast<T*>(buffer) = value;
        }

        //else the current platform is big endian; copy bytes at switched positions
        else {
            std::byte* const dst_bytes = reinterpret_cast<std::byte*>(buffer);
            const std::byte* const src_bytes = reinterpret_cast<const std::byte*>(&value);
            for (size_t i = 0; i < sizeof(T) / 2; ++i) {
                dst_bytes[i] = src_bytes[sizeof(T) - 1 - i];
                dst_bytes[sizeof(T) - 1 - i] = src_bytes[i];
            }
        }
    }


    /**
     * The default serialize function for type T, when T is a trivially copyable type.
     * @param value value to serialize.
     * @param buffer output buffer.
     */
    template <class T>
    std::enable_if_t<std::is_trivially_copyable_v<T>>
    serialize(const T& value, byte_buffer& buffer) {
        //write position
        const auto pos = buffer.size();

        //make room in the buffer
        buffer.resize(pos + sizeof(T));

        //copy the value to the buffer
        copy_value(&buffer[pos], value);
    }


    /**
     * The default deserialize function for type T, when T is a trivially copyable type.
     * @param value value to deserialize.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<std::is_trivially_copyable_v<T>>
    deserialize(T& value, const byte_buffer& buffer, byte_buffer::position& pos) {
        //check buffer
        if (pos + sizeof(T) > buffer.size()) {
            throw std::out_of_range("Buffer too small");
        }

        //copy the value from the buffer to the variable
        copy_value(&value, reinterpret_cast<const T&>(buffer[pos]));

        //update position
        pos += sizeof(T);
    }


    /**
     * The default serialize function for type T, when T is an container type.
     * @param container container to serialize.
     * @param buffer output buffer.
     */
    template <class T>
    std::enable_if_t<has_begin_end_v<T>>
    serialize(const T& container, byte_buffer& buffer) {
        serialize(container.size(), buffer);
        for (const auto& e : container) {
            serialize(e, buffer);
        }
    }


    /**
     * The default deserialize function for type T, when T is an container type.
     * @param cont container to deserialize.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<has_begin_end_v<T>>
    deserialize(T& container, const byte_buffer& buffer, byte_buffer::position& pos) {
        //read size
        typename T::size_type size;
        deserialize(size, buffer, pos);

        //read elements
        for (; size > 0; --size) {
            typename T::value_type val;
            deserialize(val, buffer, pos);

            //add element to collection
            if constexpr (has_push_back_v<T>) {
                container.push_back(std::move(val));
            }
            else if constexpr (has_insert_v<T>) {
                container.insert(std::move(val));
            }
            else {
                static_assert(false, "Don't know how to add element to container");
            }
        }
    }


    /**
     * The default serialize function for type T, when T is a tuple type.
     * @param tpl tuple to serialize.
     * @param buffer output buffer.
     */
    template <class T>
    std::enable_if_t<is_tuple_v<T> && !std::is_trivially_copyable_v<T>>
    serialize(const T& tpl, byte_buffer& buffer) {
        flatten_tuple(tpl, [&](const auto&... v) { ( serialize(v, buffer), ... ); });
    }


    /**
     * The default deserialize function for type T, when T is a tuple type.
     * @param tpl tuple to deserialize.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<is_tuple_v<T> && !std::is_trivially_copyable_v<T>>
    deserialize(T& tpl, const byte_buffer& buffer, byte_buffer::position& pos) {
        flatten_tuple(tpl, [&](auto&... v) { ( deserialize(v, buffer, pos), ... ); });
    }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_HPP
