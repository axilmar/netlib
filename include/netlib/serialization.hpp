#ifndef NETLIB_SERIALIZATION_HPP
#define NETLIB_SERIALIZATION_HPP


#include <stdexcept>
#include "byte_buffer.hpp"
#include "serialization_traits.hpp"
#include "tuple.hpp"


namespace netlib {


    /**
     * Checks if the current platform is little or big endian.
     * @return true if the current platform is little endian, false otherwise.
     */
    inline constexpr bool is_little_endian() {
        return ((const uint8_t&)uint16_t(0x0201)) == 0x01;
    }


    /**
     * If the current machine is big endian, then it switches bytes.
     * @param value value to switch bytes.
     */
    template <class T> void switch_endianess(T& value) {
        if constexpr (!is_little_endian()) {
            byte* bytes = reinterpret_cast<byte*>(&value);
            for (size_t i = 0; i < sizeof(T) / 2; ++i) {
                const byte b = bytes[i];
                bytes[i] = bytes[sizeof(T) - 1 - i];
                bytes[sizeof(T) - 1 - i] = b;
            }
        }
    }


    /**
     * Copies value to buffer, with endianess.
     * @param buffer destination buffer; must already have size.
     * @param value value to write to buffer.
     */
    template <class T> void copy_value(void* buffer, const T& value) {
        //copy with little endian; little endian is the default
        if constexpr (is_little_endian() || sizeof(T) == 1) {
            *reinterpret_cast<T*>(buffer) = value;
        }

        //else the current platform is big endian; copy bytes at switched positions
        else {
            byte* const dst_bytes = reinterpret_cast<byte*>(buffer);
            const byte* const src_bytes = reinterpret_cast<const byte*>(&value);
            for (size_t i = 0; i < sizeof(T) / 2; ++i) {
                dst_bytes[i] = src_bytes[sizeof(T) - 1 - i];
                dst_bytes[sizeof(T) - 1 - i] = src_bytes[i];
            }
        }
    }


    /**
     * The default serialize function for type T, when T is a trivially copyable type.
     * @param buffer output buffer.
     * @param value value to serialize.
     */
    template <class T>
    std::enable_if_t<std::is_trivially_copyable_v<T> && !is_tuple_v<T> && !is_pointer_v<T>>
    serialize(byte_buffer& buffer, const T& value) {
        //write position
        const auto pos = buffer.size();

        //make room in the buffer
        buffer.resize(pos + sizeof(T));

        //copy the value to the buffer
        copy_value(&buffer[pos], value);
    }


    /**
     * The default deserialize function for type T, when T is a trivially copyable type.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @param value value to deserialize.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<std::is_trivially_copyable_v<T> && !is_tuple_v<T> && !is_pointer_v<T>>
    deserialize(const byte_buffer& buffer, byte_buffer::position& pos, T& value) {
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
     * @param buffer output buffer.
     * @param container container to serialize.
     */
    template <class T>
    std::enable_if_t<has_begin_end_v<T>>
    serialize(byte_buffer& buffer, const T& container) {
        serialize(buffer, container.size());

        if constexpr ((is_little_endian() || sizeof(typename T::value_type) == 1) && std::is_trivially_copyable_v<typename T::value_type> && has_data_array_v<T>) {
            const size_t insert_position = buffer.size();
            const size_t data_size = container.size() * sizeof(typename T::value_type);
            buffer.resize(buffer.size() + data_size);
            memcpy(buffer.data() + insert_position, container.data(), data_size);
        }
        else {
            for (const auto& e : container) {
                serialize(buffer, e);
            }
        }
    }


    /**
     * The default deserialize function for type T, when T is an container type.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @param cont container to deserialize.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<has_begin_end_v<T>>
    deserialize(const byte_buffer& buffer, byte_buffer::position& pos, T& container) {
        //read size
        typename T::size_type size;
        deserialize(buffer, pos, size);

        //read elements
        if constexpr ((is_little_endian() || sizeof(typename T::value_type) == 1) && std::is_trivially_copyable_v<typename T::value_type> && has_data_array_v<T>) {
            const size_t data_size = size * sizeof(typename T::value_type);
            if (pos + data_size > buffer.size()) {
                throw std::out_of_range("Buffer too small");
            }
            const size_t insert_position = container.size();
            container.resize(container.size() + size);
            memcpy(container.data() + insert_position, buffer.data() + pos, data_size);
            pos += data_size;
        }
        else {
            for (; size > 0; --size) {
                typename T::value_type val;
                deserialize(buffer, pos, val);

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
    }


    /**
     * The default serialize function for type T, when T is a tuple type.
     * @param buffer output buffer.
     * @param tpl tuple to serialize.
     */
    template <class T>
    std::enable_if_t<is_tuple_v<T>>
    serialize(byte_buffer& buffer, const T& tpl) {
        flatten_tuple(tpl, [&](const auto&... v) { ( serialize(buffer, v), ... ); });
    }


    /**
     * The default deserialize function for type T, when T is a tuple type.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @param tpl tuple to deserialize.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<is_tuple_v<T>>
    deserialize(const byte_buffer& buffer, byte_buffer::position& pos, T& tpl) {
        flatten_tuple(tpl, [&](auto&... v) { ( deserialize(buffer, pos, v), ... ); });
    }


    /**
     * The default serialize function for type T, when T is a pointer type.
     * It serializes the object if the pointer is not null, otherwise it writes '0' to the buffer.
     * @param buffer output buffer.
     * @param ptr ptr to serialize.
     */
    template <class T>
    std::enable_if_t<is_pointer_v<T>>
    serialize(byte_buffer& buffer, const T& ptr) {
        //if the pointer is not null, write the object
        if (ptr) {
            //write true flag for non-null
            const bool v = true;
            serialize(buffer, v);

            //write object
            serialize(buffer, *ptr);
        }

        //else the pointer is null, write false
        else {
            const bool v = false;
            serialize(buffer, v);
        }
    }


    /**
     * The default deserialize function for type T, when T is a pointer type.
     * @param buffer input buffer.
     * @param pos input buffer position; on return, the next available position.
     * @param ptr ptr to object to set.
     * @exception std::out_of_range thrown if the buffer does not have enough data for the given type.
     */
    template <class T>
    std::enable_if_t<is_pointer_v<T>>
    deserialize(const byte_buffer& buffer, byte_buffer::position& pos, T& ptr) {
        //deserialize flag for null
        bool v;
        deserialize(buffer, pos, v);

        //if the value was false, then no object was written, so set pointer to null
        if (!v) {
            ptr = nullptr;
            return;
        }

        //instantiate object
        auto& object = make_object(ptr);

        //deserialize object
        deserialize(buffer, pos, object);
    }


    /**
     * Serializes multiple values.
     * @param buffer destination buffer.
     * @param value1 1st value to serialize.
     * @param value2 2nd value to serialize.
     * @param values rest of values to serialize.
     */
    template <class T1, class T2, class... T> 
    void serialize(byte_buffer& buffer, const T1& value1, const T2& value2, const T&... values) {
        serialize(buffer, value1);
        serialize(buffer, value2);
        (serialize(buffer, values), ...);
    }


    /**
     * Deserializes multiple values.
     * @param buffer source buffer.
     * @param pos source position.
     * @param value1 1st value to deserialize.
     * @param value2 2nd value to deserialize.
     * @param values rest of values to deserialize.
     */
    template <class T1, class T2, class... T>
    void deserialize(const byte_buffer& buffer, byte_buffer::position& pos, T1& value1, T2& value2, T&... values) {
        deserialize(buffer, pos, value1);
        deserialize(buffer, pos, value2);
        (deserialize(buffer, pos, values), ...);
    }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_HPP
