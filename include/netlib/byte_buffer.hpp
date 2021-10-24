#ifndef NETLIB_BYTE_BUFFER_HPP
#define NETLIB_BYTE_BUFFER_HPP


#include <vector>
#include <cstddef>


namespace netlib {


    /**
     * Byte buffer.
     */
    class byte_buffer : public std::vector<std::byte> {
    public:
        /**
         * Byte buffer position.
         */
        using position = std::vector<std::byte>::size_type;

        using std::vector<std::byte>::vector;
    };


} //namespace netlib


#endif //NETLIB_BYTE_BUFFER_HPP

