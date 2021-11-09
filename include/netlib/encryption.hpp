#ifndef NETLIB_ENCRYPTION_HPP
#define NETLIB_ENCRYPTION_HPP


#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Creates a random key, useful for the xor encryption algorithm.
     * @param size the size of the key.
     */
    byte_buffer create_random_key(const size_t size = 4096);



} //namespace netlib


#endif //NETLIB_ENCRYPTION_HPP

