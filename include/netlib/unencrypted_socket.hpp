#ifndef NETLIB_UNENCRYPTED_SOCKET_HPP
#define NETLIB_UNENCRYPTED_SOCKET_HPP


#include "socket.hpp"


namespace netlib::unencrypted {


    /**
     * Base class for unencrypted sockets.
     */
    class socket : public netlib::socket {
    public:
        /**
         * The default constructor.
         * @param handle optional handle.
         */
        socket(handle_type handle = invalid_handle) : m_handle(handle) {
        }

        /**
         * The move constructor.
         * @param src source object.
         */
        socket(socket&& src) : m_handle(src.m_handle) {
            src.m_handle = invalid_handle;
        }

        /**
         * Closes the underlying socket.
         */
        ~socket();

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

        /**
         * Returns the handle.
         */
        handle_type handle() const override {
            return m_handle;
        }

    private:
        handle_type m_handle;
    };


} //namespace netlib::unencrypted


#endif //NETLIB_UNENCRYPTED_SOCKET_HPP
