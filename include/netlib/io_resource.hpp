#ifndef NETLIB_IO_RESOURCE_HPP
#define NETLIB_IO_RESOURCE_HPP


#include <utility>


namespace netlib {


    /**
     * Base class for input/output resources.
     */
    class io_resource {
    public:
        /**
         * Result type : pair of size of bytes read/written, open boolean flag.
         */
        using result = std::pair<size_t, bool>;

        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~io_resource() {}

        /**
         * Interface for reading data from a resource.
         * @param buffer destination buffer.
         * @param size number of bytes to read.
         * @return number of bytes read or closed.
         */
        virtual result read(void* buffer, size_t size) = 0;

        /**
         * Interface for writing data to a resource.
         * @param buffer source buffer.
         * @param size number of bytes to write.
         * @return number of bytes written or closed.
         */
        virtual result write(const void* buffer, size_t size) = 0;
    };


} //namespace netlib


#endif //NETLIB_IO_RESOURCE_HPP
