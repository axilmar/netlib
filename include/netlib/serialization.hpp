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
            //the write position is at the end of the buffer
            const size_t write_index = buffer.size();

            //make room in the destination buffer
            buffer.resize(sizeof(T));

            //write data at this position
            auto* write_pos = buffer.data() + write_index;

            //copy without endianess change
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                *reinterpret_cast<T*>(write_pos) = value;
            }

            //else copy data with endianess change
            else {
                for (size_t i = 0; i < sizeof(T); ++i) {
                    write_pos[i] = reinterpret_cast<const typename Buffer::value_type*>(&value)[sizeof(T) - 1 - i];
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

            //copy without endianess change
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                value = *reinterpret_cast<const T*>(read_pos);
            }

            //else copy data with endianess change
            else {
                for (size_t i = 0; i < sizeof(T); ++i) {
                    reinterpret_cast<typename Buffer::value_type*>(&value)[i] = write_pos[sizeof(T) - 1 - i];
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
            buffer.resize(sizeof(T) * array_size);

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
            buffer.at(pos + sizeof(T) * array_size - 1);

            //read position
            const auto* read_pos = &buffer[pos];

            //copy without endianess change
            if constexpr (is_little_endian() == (NETLIB_NETWORK_ENDIANESS_LITTLE) || sizeof(T) == 1) {
                memcpy(values, read_pos, sizeof(T) * array_size);
            }

            //else copy data with endianess change
            else {
                for (T* value = values; value < values + array_size; ++value, write_pos += sizeof(T)) {
                    for (size_t i = 0; i < sizeof(T); ++i) {
                        reinterpret_cast<typename Buffer::value_type*>(value)[i] = write_pos[sizeof(T) - 1 - i];
                    }
                }
            }

            //update the position
            pos += sizeof(T);
        }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_HPP
