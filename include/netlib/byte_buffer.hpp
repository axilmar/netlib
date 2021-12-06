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
     * 
     * @param size required buffer size.
     * @return the buffer.
     */
    byte_buffer& temp_byte_buffer(size_t size = 0);


    /**
     * Returns a temp byte buffer with the specific content.
     * @param begin begin of content.
     * @param end end of content.
     * @return buffer.
     */
    template <class T> byte_buffer& temp_byte_buffer(const T& begin, const T& end) {
        byte_buffer& buffer = temp_byte_buffer(0);
        buffer.insert(buffer.end(), begin, end);
        return buffer;
    }


} //namespace netlib


#endif //NETLIB_BYTE_BUFFER_HPP
