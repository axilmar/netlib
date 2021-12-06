#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include "socket_handle.hpp"
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Base class for sockets. 
     */
    class socket {
    public:
        /**
         * The destructor.
         * It closes the socket.
         */
        ~socket();

        /**
         * Returns the socket handle.
         * @return the socket handle.
         */
        socket_handle handle() const { return m_handle; }

        /**
         * Checks if this socket is valid or invalid.
         * @return true if valid, false if invalid.
         */
        operator bool() const;

    protected:
        /**
         * The default constructor.
         * The socket handle is initialized to an invalid socket value.
         */
        socket();

        /**
         * Constructor from handle. 
         * @param handle the socket's handle. 
         */
        socket(socket_handle handle);

        /**
         * Sockets are not copyable. 
         */
        socket(const socket&) = delete;

        /**
         * The move constructor.
         * @param src source object.
         */
        socket(socket&& src);

        /**
         * Sockets are not copyable.
         */
        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

    private:
        //handle
        socket_handle m_handle;

        //closes the socket.
        void close();
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
