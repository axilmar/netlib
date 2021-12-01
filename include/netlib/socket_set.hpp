#ifndef NETLIB_SOCKET_SET_HPP
#define NETLIB_SOCKET_SET_HPP


#include <set>
#include "socket.hpp"


namespace netlib {


    /**
     * Wrapper over fd_set.
     * Used with select().
     */
    class socket_set {
    public:
        /**
         * max number of sockets per socket set.
         */
        static constexpr size_t max_sockets = 1024;

        /**
         * size of the internal buffer.
         */
        static constexpr size_t data_size = sizeof(uintptr_t) * max_sockets + sizeof(size_t);

        /**
         * The default constructor.
         */
        socket_set();

        /**
         * Returns the set of sockets.
         */
        const std::set<socket>& sockets() const { return m_sockets; }

        /**
         * Adds a socket.
         * @param s socket to add.
         * @exception std::invalid_argument thrown if the socket is already in the set.
         */
        void add(socket&& s);

        /**
         * Removes a socket.
         * @param s socket to remove.
         * @exception std::invalid_argument thrown if the socket is not in this set.
         */
        void remove(socket& s);

        /**
         * Returns the internal data.
         * The internal data are used as an fd_set buffer.
         */
        const void* data() const { return m_data; }

        /**
         * Returns the internal data.
         * The internal data are used as an fd_set buffer.
         */
        void* data() { return m_data; }

    private:
        std::set<socket> m_sockets;
        char m_data[data_size];
    };


} //namespace netlib


#endif //NETLIB_SOCKET_SET_HPP

