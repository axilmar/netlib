#include "netlib/platform.hpp"
#include <stdexcept>
#include "netlib/pipe.hpp"


#define READ_HANDLE m_fds[0]
#define WRITE_HANDLE m_fds[1]


namespace netlib {


    //The default constructor.
    pipe::pipe(size_t capacity) {
        //create the pipe
        if (!create_pipe(m_fds, capacity)) {
            throw std::runtime_error(get_last_error_message());
        }
    }


    //The move constructor.
    pipe::pipe(pipe&& p) {
        //get handles from given pipe
        READ_HANDLE = p.READ_HANDLE;
        WRITE_HANDLE = p.WRITE_HANDLE;

        //reset given pipe
        std::lock_guard lock(p.m_mutex);
        p.READ_HANDLE = -1;
        p.WRITE_HANDLE = -1;
    }


    //closes the pipe
    pipe::~pipe() {
        std::lock_guard lock(m_mutex);
        close_pipe(m_fds);
    }


    //The move assignment operator.
    pipe& pipe::operator = (pipe&& p) {
        if (&p == this) {
            return *this;
        }

        std::scoped_lock locks(m_mutex, p.m_mutex);

        //set this' handles from input pipe handles
        READ_HANDLE = p.READ_HANDLE;
        WRITE_HANDLE = p.WRITE_HANDLE;

        //reset input pipe handles
        p.READ_HANDLE = -1;
        p.WRITE_HANDLE = -1;

        return *this;
    }


    /**
     * Returns the write handle.
     */
    io_resource::handle_type pipe::write_handle() const {
        std::shared_lock lock(m_mutex);
        return WRITE_HANDLE;
    }


    /**
     * Returns the read handle.
     */
    io_resource::handle_type pipe::read_handle() const {
        std::shared_lock lock(m_mutex);
        return READ_HANDLE;
    }


    //write data to the pipe.
    io_resource::io_result_type pipe::write(const void* buffer, size_t size) {
        //check the size against the return type
        if (size > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Size too big to cast it to 'int'.");
        }

        //get the write handle synchronized
        uintptr_t write_handle = this->write_handle();

        //write
        bool pipe_is_open;
        int bytes_written = ::pipe_write(write_handle, buffer, static_cast<int>(size), pipe_is_open);

        //success/graceful close
        if (bytes_written >= 0) {
            return { bytes_written, pipe_is_open };
        }

        //throw error
        throw std::runtime_error(get_last_error_message());
    }


    //read data from the pipe
    io_resource::io_result_type pipe::read(void* buffer, size_t size) {
        //check the size against the return type
        if (size > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Size too big to cast it to 'int'.");
        }


        //get the read handle synchronized
        uintptr_t read_handle = this->read_handle();

        //read
        bool pipe_is_open;
        int bytes_read = pipe_read(read_handle, buffer, static_cast<int>(size), pipe_is_open);

        //success/graceful close
        if (bytes_read >= 0) {
            return { bytes_read, pipe_is_open };
        }

        //throw error
        throw std::runtime_error(get_last_error_message());
    }


} //namespace netlib
