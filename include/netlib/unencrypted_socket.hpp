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
         * Closes the underlying socket.
         */
        ~socket();

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
