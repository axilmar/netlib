#include "netlib/byte_buffer.hpp"


namespace netlib {


    //Makes sure the buffer has enough size.
    void byte_buffer::ensure_size(size_t size) {
        if (size > this->size()) {
            resize(size);
        }
    }


} //namespace netlib
