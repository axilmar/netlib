#ifndef NETLIB_CRC32_HPP
#define NETLIB_CRC32_HPP


#include <cstdint>


namespace netlib {


    /**
     * Computes the crc32 of the given buffer.
     * @param buffer buffer.
     * @param size size of buffer, in bytes.
     * @return crc32 of the buffer.
     */
    uint32_t compute_crc32(const void* const buffer, const size_t size);


} //namespace netlib


#endif //NETLIB_CRC32_HPP
