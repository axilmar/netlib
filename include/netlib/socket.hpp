#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include "socket_handle.hpp"


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
        virtual ~socket();

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

    protected:
        /**
         * The constructor. 
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

    private:
        socket_handle m_handle;

        //closes the socket.
        void close();
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
