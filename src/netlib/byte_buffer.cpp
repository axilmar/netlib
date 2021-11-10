#include "netlib/byte_buffer.hpp"


namespace netlib {


    //The default constructor.
    byte_buffer::byte_buffer() {
    }


    //Constructor from size.
    byte_buffer::byte_buffer(size_t size) 
        : std::vector<byte>(size)
    {
    }


    //constructor from bytes.
    byte_buffer::byte_buffer(const byte* bytes) {
        if (bytes) {
            for (; *bytes; ++bytes) {
                push_back(*bytes);
            }
        }
    }


    //constructor from bytes and size.
    byte_buffer::byte_buffer(const byte* bytes, size_t size) 
        : std::vector<byte>(bytes, bytes + size)
    {
    }


    //Makes sure the buffer has enough size.
    void byte_buffer::ensure_size(size_t size) {
        if (size > this->size()) {
            resize(size);
        }
    }


} //namespace netlib
