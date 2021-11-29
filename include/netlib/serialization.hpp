#ifndef NETLIB_SERIALIZATION_HPP
#define NETLIB_SERIALIZATION_HPP


#include <type_traits>


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
     * Serializes a trivial value into a buffer.
     * It copies the bytes of the value into the buffer, either in little endian or in big endian order,
     * depending on configuration and platform endianess.
     * @param buffer std::vector-like buffer to store the value; its value type size must be 1.
     * @param value the value to serialize.
     */
    template <class Buffer, class T> std::enable_if_t<sizeof(typename Buffer::value_type) == 1 && std::is_trivial_v<T>> 
        serialize(Buffer& buffer, const T& value) {
            //if size of T is 1 byte, put it directly in the buffer, no need to check for endianess
            if constexpr (sizeof(T) == 1) {
                buffer.push_back(static_cast<typename Buffer::value_type>(value));
            }

            //else size of T is greater than 1 byte, so check for endianess.
            else {
                //the write position is at the end of the buffer
                const size_t write_index = buffer.size();

                //make room in the destination buffer
                buffer.resize(buffer.size() + sizeof(T));

                //write data at this position
                auto* write_pos = buffer.data() + write_index;

                //copy without endianess change
                if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE)) {
                    *reinterpret_cast<T*>(write_pos) = value;
                }

                //else copy data with endianess change
                else {
                    for (size_t i = 0; i < sizeof(T); ++i) {
                        write_pos[i] = reinterpret_cast<const typename Buffer::value_type*>(&value)[sizeof(T) - 1 - i];
                    }
                }
            }
        }


    /**
     * Deserializes a trivial value from a buffer.
     * It copies the bytes of the buffer into the value, either in little endian or in big endian order,
     * depending on configuration and platform endianess.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param pos current position into the buffer; at return, the next available position.
     * @param value the value to deserialize.
     * @exception std::out_of_range thrown if the position points beyond the last byte of the buffer.
     */
    template <class Buffer, class T> std::enable_if_t<sizeof(typename Buffer::value_type) == 1 && std::is_trivial_v<T>> 
        deserialize(const Buffer& buffer, size_t& pos, T& value) {
            //read position; throws exception if invalid
            const auto* read_pos = &buffer.at(pos);

            //copy without endianess change or if the size of T is 1
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                value = *reinterpret_cast<const T*>(read_pos);
            }

            //else copy data with endianess change
            else {
                for (size_t i = 0; i < sizeof(T); ++i) {
                    reinterpret_cast<typename Buffer::value_type*>(&value)[i] = read_pos[sizeof(T) - 1 - i];
                }
            }

            //update the position
            pos += sizeof(T);
        }


    /**
     * Serializes an array of trivial values into a buffer.
     * It copies the bytes of the values into the buffer, either in little endian or in big endian order,
     * depending on configuration and platform endianess.
     * @param buffer std::vector-like buffer to store the values; its value type size must be 1.
     * @param values the values to serialize.
     * @param array_size number of elements in the array.
     */
    template <class Buffer, class T> std::enable_if_t<sizeof(typename Buffer::value_type) == 1 && std::is_trivial_v<T>>
        serialize(Buffer& buffer, const T* values, const size_t array_size) {
            //the write position is at the end of the buffer
            const size_t write_index = buffer.size();

            //make room in the destination buffer
            buffer.resize(buffer.size() + sizeof(T) * array_size);

            //write data at this position
            auto* write_pos = buffer.data() + write_index;

            //copy without endianess change
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                memcpy(write_pos, values, sizeof(T) * array_size);
            }

            //else copy data with endianess change
            else {
                for (const T* value = values; value < values + array_size; ++value, write_pos += sizeof(T)) {
                    for (size_t i = 0; i < sizeof(T); ++i) {
                        write_pos[i] = reinterpret_cast<const typename Buffer::value_type*>(value)[sizeof(T) - 1 - i];
                    }
                }
            }
        }


    /**
     * Deserializes an array of trivial values from a buffer.
     * It copies the bytes of the buffer into the values, either in little endian or in big endian order,
     * depending on configuration and platform endianess.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param pos current position into the buffer; at return, the next available position.
     * @param values the values to deserialize.
     * @param array_size number of elements in the array.
     * @exception std::out_of_range thrown if the position points beyond the last byte of the buffer.
     */
    template <class Buffer, class T> std::enable_if_t<sizeof(typename Buffer::value_type) == 1 && std::is_trivial_v<T>>
        deserialize(const Buffer& buffer, size_t& pos, T* values, const size_t array_size) {
            //check if the end position is within the buffer
            if (pos + sizeof(T) * array_size > buffer.size()) {
                throw std::runtime_error("Cannot deserialize array; the buffer does not contain enough data.");
            }

            //read position
            const auto* read_pos = &buffer[pos];

            //copy without endianess change
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                memcpy(values, read_pos, sizeof(T) * array_size);
            }

            //else copy data with endianess change
            else {
                for (T* value = values; value < values + array_size; ++value, read_pos += sizeof(T)) {
                    for (size_t i = 0; i < sizeof(T); ++i) {
                        reinterpret_cast<typename Buffer::value_type*>(value)[i] = read_pos[sizeof(T) - 1 - i];
                    }
                }
            }

            //update the position
            pos += sizeof(T) * array_size;
        }


    /**
     * Serializes a boolean value as a byte.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param value value to serialize.
     */
    template <class Buffer> void serialize(Buffer& buffer, bool value) {
        serialize(buffer, char(value ? 1 : 0));
    }


    /**
     * Deserializes a boolean value as a byte.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param value value to deserialize.
     * @param pos current position into the buffer; at return, the next available position.
     * @exception std::out_of_range thrown if the position points beyond the last byte of the buffer.
     */
    template <class Buffer> void deserialize(const Buffer& buffer, size_t& pos, bool& value) {
        char c;
        deserialize(buffer, pos, c);
        value = c ? true : false;
    }


    /**
     * Serializes a boolean array as bits.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param values values to serialize.
     * @param array_size size of array.
     */
    template <class Buffer> void serialize(Buffer& buffer, const bool* values, const size_t array_size) {
        //find size of required bytes
        const size_t byte_size = (array_size + CHAR_BIT - 1) / CHAR_BIT;

        //make room in the buffer
        const size_t write_index = buffer.size();
        buffer.resize(buffer.size() + byte_size);
        char* write_pos = buffer.data() + write_index;

        //write bits
        for (const bool* array_end = values + array_size; values < array_end; ++write_pos) {
            for (size_t bit_index = 0; bit_index < CHAR_BIT && values < array_end; ++bit_index, ++values) {
                if (*values) {
                    *write_pos |= 1 << bit_index;
                }
            }
        }
    }


    /**
     * Deserializes a boolean array from bits.
     * @param buffer std::vector-like buffer to that contains the data; its value type size must be 1.
     * @param pos current position into the buffer; at return, the next available position.
     * @param values values to deserialize.
     * @param array_size size of array.
     */
    template <class Buffer> void deserialize(Buffer& buffer, size_t& pos, bool* values, const size_t array_size) {
        //find size of required bytes
        const size_t byte_size = (array_size + CHAR_BIT - 1) / CHAR_BIT;

        //check if the buffer contains enough data
        if (pos + byte_size > buffer.size()) {
            throw std::runtime_error("Cannot deserialize array of bools; the buffer does not contain enough data.");
        }

        //find read position
        const char* read_pos = buffer.data() + pos;

        //read bits
        for (bool* array_end = values + array_size; values < array_end; ++read_pos) {
            for (size_t bit_index = 0; bit_index < CHAR_BIT && values < array_end; ++bit_index, ++values) {
                *values = (*read_pos & (1 << bit_index)) ? true : false;
            }
        }

        //update position
        pos += byte_size;
    }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_HPP
