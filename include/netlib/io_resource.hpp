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
        using io_result_type = std::pair<size_t, bool>;

        /**
         * handle type.
         */
        using handle_type = uintptr_t;

        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~io_resource() {}

        /**
         * Returns the write handle.
         */
        virtual handle_type write_handle() const = 0;

        /**
         * Returns the read handle.
         */
        virtual handle_type read_handle() const = 0;

        /**
         * Returns true if the resource is stream-oriented, false otherwise.
         */
        virtual bool is_stream_oriented() const = 0;

        /**
         * Interface for reading data from a resource.
         * @param buffer destination buffer.
         * @param size number of bytes to read.
         * @return number of bytes read or closed.
         */
        virtual io_result_type read(void* buffer, size_t size) = 0;

        /**
         * Interface for writing data to a resource.
         * @param buffer source buffer.
         * @param size number of bytes to write.
         * @return number of bytes written or closed.
         */
        virtual io_result_type write(const void* buffer, size_t size) = 0;
    };


} //namespace netlib


#endif //NETLIB_IO_RESOURCE_HPP
