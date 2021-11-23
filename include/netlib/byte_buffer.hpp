#ifndef NETLIB_BYTE_BUFFER_HPP
#define NETLIB_BYTE_BUFFER_HPP


#include <vector>
#include <cstddef>
#include "byte.hpp"


namespace netlib {


    /**
     * Byte buffer.
     */
    class byte_buffer : public std::vector<byte> {
    public:
        /**
         * Byte buffer position.
         */
        using position = std::vector<byte>::size_type;

        /**
         * The default constructor.
         */
        byte_buffer();

        /**
         * Constructor from size.
         * @param size size.
         */
        byte_buffer(size_t size);

        /**
         * constructor from bytes.
         * @param bytes bytes; null terminated string.
         */
        byte_buffer(const byte* bytes);

        /**
         * constructor from bytes and size.
         * @param bytes bytes.
         * @param size size.
         */
        byte_buffer(const byte* bytes, size_t size);

        /**
         * Constructor from iterator range.
         * @param begin iterator to beginning of range.
         * @param end iterator to end of range.
         */
        template <class It>
        byte_buffer(const It& begin, const It& end) : std::vector<byte>(begin, end) {}

        /**
         * Makes sure the buffer has enough size.
         * @param size size.
         */
        void ensure_size(size_t size);
    };


} //namespace netlib


#endif //NETLIB_BYTE_BUFFER_HPP

