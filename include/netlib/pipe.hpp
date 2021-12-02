#ifndef NETLIB_PIPE_HPP
#define NETLIB_PIPE_HPP


namespace netlib {


    /**
     * A bidirectional communication in-kernel binary data channel.  
     */
    class pipe {
    public:
        /**
         * constant that indicates the returned size was invalid. 
         */
        static constexpr size_t nsize = ~size_t{0};

        /**
         * The default constructor. 
         * @exception std::runtime_error thrown if there was an error creating the pipe.
         */
        pipe();

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
         * Writes data to the pipe.
         * @param buffer pointer to data to write.
         * @param size number of bytes to write.
         * @return number of bytes written or pipe::nsize if the pipe is closed.
         * @exception std::runtime_error thrown if there was an error.
         * @exception std::invalid_argument thrown if size is too big for the operation.
         */
        size_t write(const void* buffer, size_t size);

        /**
         * Reads data from the pipe.
         * @param buffer pointer to data to receive.
         * @param size number of bytes to receive.
         * @return number of bytes received or pipe::nsize if the pipe is closed.
         * @exception std::runtime_error thrown if there was an error.
         * @exception std::invalid_argument thrown if size is too big for the operation.
         */
        size_t read(void* buffer, size_t size);

        /**
         * Returns the read descriptor. 
         */
        int get_read_descriptor() const { return m_read_fd; }

        /**
         * Returns the write descriptor.
         */
        int get_write_descriptor() const { return m_write_fd; }

        /**
         * Closes the read descriptor. 
         */
        void close_read_descriptor();

        /**
         * Closes the write descriptor. 
         */
        void close_write_descriptor();

    private:
        int m_read_fd;
        int m_write_fd;
    };


} //namespace netlib


#endif //NETLIB_PIPE_HPP
