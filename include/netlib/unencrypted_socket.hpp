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
         * An invalid socket is created.
         */
        socket();

        /**
         * Constructor from handle.
         * @param handle handle.
         * @exception std::system_error if the socket is invalid.
         */
        socket(handle_type handle);

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

    protected:
        /**
         * Sets a new handle.
         * The old handle is closed.
         */
        void set_handle(handle_type handle);

    private:
        handle_type m_handle;
    };


} //namespace netlib::unencrypted


#endif //NETLIB_UNENCRYPTED_SOCKET_HPP
