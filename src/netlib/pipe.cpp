#include "netlib/platform.hpp"
#include <stdexcept>
#include "netlib/pipe.hpp"


namespace netlib {


    //The default constructor.
    pipe::pipe() {
        //create the pipe
        int fd[2];
        int error = ::pipe(fd);
        
        //success
        if (!error) {
            m_read_fd = fd[0];
            m_write_fd = fd[1];
            return;
        }

        //throw error
        throw std::runtime_error(get_last_error_message());
    }


    //The move constructor.
    pipe::pipe(pipe&& p) : m_read_fd(p.m_read_fd), m_write_fd(p.m_write_fd) {
        p.m_read_fd = -1;
        p.m_write_fd = -1;
    }


    //closes the pipe
    pipe::~pipe() {
        close_read_descriptor();
        close_write_descriptor();
    }


    //The move assignment operator.
    pipe& pipe::operator = (pipe&& p) {
        int temp_read_fd = p.m_read_fd;
        int temp_write_fd = p.m_write_fd;

        p.m_read_fd = -1;
        p.m_write_fd = -1;

        m_read_fd = temp_read_fd;
        m_write_fd = temp_write_fd;
        
        return *this;
    }


    //write data to the pipe.
    size_t pipe::write(const void* buffer, size_t size) {
        //check the size against the return type
        if (size > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Size too big to return it as 'int'.");
        }

        //check the size against unsigned int
        if (size > std::numeric_limits<unsigned int>::max()) {
            throw std::invalid_argument("Size too big to pass to the 'write' function.");
        }

        //check the write handle
        if (m_write_fd == -1) {
            return nsize;
        }

        //write
        errno = 0;
        int bytes_written = ::write(m_write_fd, buffer, static_cast<unsigned int>(size));

        //success
        if (bytes_written >= 0) {
            return bytes_written;
        }

        //if error is 'the other end is closed', close this too
        if (errno == EPIPE) {
            close_write_descriptor();
            return nsize;
        }

        //throw error
        throw std::runtime_error(get_last_error_message());
    }


    //read data from the pipe
    size_t pipe::read(void* buffer, size_t size) {
        //check the size against the return type
        if (size > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Size too big to return it as 'int'.");
        }

        //check the size against unsigned int
        if (size > std::numeric_limits<unsigned int>::max()) {
            throw std::invalid_argument("Size too big to pass to the 'read' function.");
        }

        //check the read handle
        if (m_read_fd == -1) {
            return nsize;
        }

        //read
        errno = 0;
        int bytes_read = ::read(m_read_fd, buffer, static_cast<unsigned int>(size));

        //success
        if (bytes_read >= 0) {
            return bytes_read;
        }

        //if error is 'the other end is closed', close this too
        if (errno == EPIPE) {
            close_read_descriptor();
            return nsize;
        }

        //throw error
        throw std::runtime_error(get_last_error_message());
    }


    //Closes the read descriptor.
    void pipe::close_read_descriptor() {
        if (m_read_fd != -1) {
            close(m_read_fd);
            m_read_fd = -1;
        }
    }


    //Closes the write descriptor.
    void pipe::close_write_descriptor() {
        if (m_write_fd != -1) {
            close(m_write_fd);
            m_write_fd = -1;
        }
    }


} //namespace netlib
