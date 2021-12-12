#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <cstdint>
#include <atomic>
#include "socket_address.hpp"


namespace netlib {


    /**
     * Base class for sockets.
     * 
     * It internally uses reference count to manage the socket lifetime.
     * This means the socket class is a value class, and can be easily
     * used in many complex multithreaded scenarios.
     * 
     * The class is not thread safe when updated via operator =,
     * following std::shared_ptr semantics.
     */
    class socket {
    public:
        /**
         * handle type.
         */
        using handle_type = uintptr_t;

        /**
         * Invalid handle.
         */
        static constexpr handle_type invalid_handle = ~handle_type(0);

        /**
         * The default constructor.
         * @param handle socket handle.
         */
        socket(handle_type handle = invalid_handle);

        /**
         * The copy constructor. 
         * @param src source object.
         */
        socket(const socket& src);

        /**
         * The move constructor. 
         * @param src source object.
         */
        socket(socket&& src);

        /**
         * The destructor.
         * closes the socket if its reference count drops to 0.
         */
        ~socket();

        /**
         * The copy assignment operator. 
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (const socket& src);

        /**
         * The move assignment operator. 
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

        /**
         * Returns the handle.
         */
        handle_type handle() const;

        /**
         * Returns true if the socket is valid, false otherwise.
         */
        explicit operator bool() const;

        /**
         * Returns the address this socket is bound to.
         */
        socket_address bound_address() const;

    private:
        //internal reference-counted socket handle structure
        struct data;

        //data
        data* m_data;

        //increments the ref count
        void ref();

        //decrements the ref count and deletes the socket/data block if ref count reaches 0
        void unref();
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
