#ifndef NETLIB_PIPE_HPP
#define NETLIB_PIPE_HPP


#include <shared_mutex>
#include "io_resource.hpp"


/**
 * Default pipe size.
 */
#ifndef NETLIB_DEFAULT_PIPE_SIZE
#define NETLIB_DEFAULT_PIPE_SIZE 65536
#endif


#ifdef _WIN32
inline constexpr bool pipes_are_stream_oriented = false;
#endif


#ifdef linux
inline constexpr bool pipes_are_stream_oriented = true;
#endif


namespace netlib {


    /**
     * A bidirectional communication in-kernel binary data channel.  
     */
    class pipe : public io_resource {
    public:
        /**
         * constant that indicates the returned size was invalid. 
         */
        static constexpr size_t nsize = ~size_t{0};

        /**
         * The default constructor. 
         * @exception std::runtime_error thrown if there was an error creating the pipe.
         */
        pipe(size_t capacity = NETLIB_DEFAULT_PIPE_SIZE);

        /**
         * The copy constructor. 
         * The object is not copyable due to file descriptors. 
         */
        pipe(const pipe&) = delete;

        /**
         * The move constructor.
         * @param p source object.
         */
        pipe(pipe&& p);

        /**
         * The destructor.
         * It closes the pipe.
         */
        ~pipe();

        /**
         * The copy assignment operator. 
         * The object is not copyable due to file descriptors.
         */
        pipe& operator = (const pipe&) = delete;

        /**
         * The move assignment operator.
         * @param p source object.
         * @return reference to this.
         */
        pipe& operator = (pipe&& p);

        /**
         * Returns the write handle.
         */
        handle_type write_handle() const override;

        /**
         * Returns the read handle.
         */
        handle_type read_handle() const override;

        /**
         * Returns true, is the pipe supports stream mode, otherwise false.
         */
        bool is_stream_oriented() const override { return pipes_are_stream_oriented; }

        /**
         * Writes data to the pipe.
         * @param buffer pointer to data to write.
         * @param size number of bytes to write.
         * @return result of operation.
         * @exception std::runtime_error thrown if there was an error.
         * @exception std::invalid_argument thrown if size is too big for the operation.
         */
        io_result_type write(const void* buffer, size_t size) override;

        /**
         * Reads data from the pipe.
         * @param buffer pointer to data to receive.
         * @param size number of bytes to receive.
         * @return result of operation.
         * @exception std::runtime_error thrown if there was an error.
         * @exception std::invalid_argument thrown if size is too big for the operation.
         */
        io_result_type read(void* buffer, size_t size) override;

    private:
        mutable std::shared_mutex m_mutex;
        uintptr_t m_fds[2];
    };


} //namespace netlib


#endif //NETLIB_PIPE_HPP
