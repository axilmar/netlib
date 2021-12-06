#include <memory_resource>
#include "netlib/byte_buffer.hpp"


namespace netlib {


    //get temp byte buffer.
    byte_buffer& temp_byte_buffer(size_t size) {
        static thread_local std::pmr::unsynchronized_pool_resource memory_pool;
        static thread_local byte_buffer buffer{ std::pmr::polymorphic_allocator<char>(&memory_pool) };
        buffer.resize(size);
        return buffer;
    }


} //namespace netlib
