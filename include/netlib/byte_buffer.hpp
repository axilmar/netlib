#ifndef NETLIB_BYTE_BUFFER_HPP
#define NETLIB_BYTE_BUFFER_HPP


#include <vector>


namespace netlib {


    /**
     * Byte buffer. 
     */
    using byte_buffer = std::vector<char, std::pmr::polymorphic_allocator<char>>;


    /**
     * Returns a temporary thread-local byte buffer,
     * which can be used to send and receive data 
     * from the current thread.
     *
     * The buffer uses an unsynchronized memory resource,
     * since each thread has its own version of the buffer.
     */
    inline byte_buffer& temp_byte_buffer();


} //namespace netlib


#endif //NETLIB_BYTE_BUFFER_HPP
