#ifndef NETLIB_SOCKET_SET_HPP
#define NETLIB_SOCKET_SET_HPP


#include <array>
#include <map>
#include "byte.hpp"


namespace netlib {


    class socket;


    /**
     * Set of sockets allowing the reading/writing/error checking using select.
     */
    class socket_set {
    public:
        /**
         * max sockets.
         */
        static constexpr size_t MAX_SOCKETS = 1024;

        /**
         * Buffer size.
         */
        static constexpr size_t BUFFER_SIZE = 10 * 1024;

        /**
         * Initializes the socket set.
         */
        socket_set();

        /**
         * checks if the set is empty.
         */
        bool empty() const { return m_socket_map.empty(); }

        /**
         * Checks if it includes the socket.
         * @param s socket to check.
         * @return true if the socket is included in the set, false otherwise.
         */
        bool contains(socket& s) const;

        /**
         * adds a socket.
         * @param s socket to add.
         * @exception socket_error if the socket has already been added or no more sockets can be added.
         */
        void add(socket& s);

        /**
         * removes a socket.
         * @param s socket to remove.
         * @exception socket_error if the socket has already been removed or it does not exist.
         */
        void remove(socket& s);

        /**
         * clears the socket set.
         */
        void clear();

        /**
         * Returns the data.
         */
        const void* data() const { return m_data; }

        /**
         * Returns the data.
         */
        void* data() { return m_data; }

        /**
         * Returns the size.
         */
        constexpr size_t size() const { return BUFFER_SIZE; }

        /**
         * Returns the socket map.
         */
        const std::map<uintptr_t, socket*>& get_socket_map() const { return m_socket_map; }

    private:
        byte m_data[BUFFER_SIZE];
        std::map<uintptr_t, socket*> m_socket_map;

        friend class socket_selector;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_SET_HPP
