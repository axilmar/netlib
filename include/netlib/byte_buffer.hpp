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
        byte_buffer() {
        }

        /**
         * Constructor from size.
         * @param size size.
         */
        byte_buffer(size_t size) : std::vector<byte>(size) {
        }

        /**
         * Makes sure the buffer has enough size.
         * @param size size.
         */
        void ensure_size(size_t size);
    };


} //namespace netlib


#endif //NETLIB_BYTE_BUFFER_HPP

