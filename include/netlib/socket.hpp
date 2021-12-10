#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <cstdint>
#include "socket_address.hpp"


namespace netlib {


    /**
     * Base class for sockets. 
     */
    class socket {
    public:
        /**
         * The destructor. 
         * closes the socket. 
         */
        ~socket();

        /**
         * Returns true if the socket is valid, false otherwise. 
         */
        explicit operator bool() const;

        /**
         * Returns the address this socket is bound to.
         */
        socket_address bound_address() const;

    protected:
        /**
         * The default constructor.
         * @param handle socket handle.
         */
        socket(uintptr_t handle = ~uintptr_t(0));

        /**
         * The copy constructor. 
         * Not copyable. 
         */
        socket(const socket&) = delete;

        /**
         * The move constructor. 
         * Moves the given socket to this.
         */
        socket(socket&& src);

        /**
         * The copy assignment operator. 
         * Not copyable.
         */
        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator. 
         * Moves the given socket to this.
         */
        socket& operator = (socket&& src);

        /**
         * Returns the handle.
         */
        uintptr_t handle() const { return m_handle; }

    private:
        uintptr_t m_handle;

        friend class socket_poller;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
