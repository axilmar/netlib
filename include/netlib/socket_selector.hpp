#ifndef NETLIB_SOCKET_SELECTOR_HPP
#define NETLIB_SOCKET_SELECTOR_HPP


#include <shared_mutex>
#include <vector>
#include <chrono>
#include "socket_set.hpp"


namespace netlib {


    /**
     * socket selector.
     * Thread-safe class.
     */
    class socket_selector {
    public:
        /**
         * Returns true if it contains the given socket for the given operation.
         * @param socket to check.
         * @return true if the socket is in the given set, false otherwise.
         */
        bool contains_socket_for_reading(socket& s) const;

        /**
         * Adds a socket for reading.
         * @param s socket to add.
         * @exception socket_error thrown if the socket is already in the particular set.
         */
        void add_socket_for_reading(socket& s);

        /**
         * Removes a socket for reading.
         * @param s socket to remove.
         * @exception socket_error thrown if the socket is not in the particular set.
         */
        void remove_socket_for_reading(socket& s);

        /**
         * Returns true if it contains the given socket for the given operation.
         * @param socket to check.
         * @return true if the socket is in the given set, false otherwise.
         */
        bool contains_socket_for_writing(socket& s) const;

        /**
         * Adds a socket for writing.
         * @param s socket to add.
         * @exception socket_error thrown if the socket is alwritey in the particular set.
         */
        void add_socket_for_writing(socket& s);

        /**
         * Removes a socket for writing.
         * @param s socket to remove.
         * @exception socket_error thrown if the socket is not in the particular set.
         */
        void remove_socket_for_writing(socket& s);

        /**
         * Returns true if it contains the given socket for the given operation.
         * @param socket to check.
         * @return true if the socket is in the given set, false otherwise.
         */
        bool contains_socket_for_error_checking(socket& s) const;

        /**
         * Adds a socket for excepting.
         * @param s socket to add.
         * @exception socket_error thrown if the socket is alexcepty in the particular set.
         */
        void add_socket_for_error_checking(socket& s);

        /**
         * Removes a socket for excepting.
         * @param s socket to remove.
         * @exception socket_error thrown if the socket is not in the particular set.
         */
        void remove_socket_for_error_checking(socket& s);

        /**
         * Adds a socket set for reading.
         * @param set set to add.
         */
        void add_socket_set_for_reading(const socket_set& set);

        /**
         * Removes a socket set for reading.
         * @param set set to remove.
         */
        void remove_socket_set_for_reading(const socket_set& set);

        /**
         * Empties the socket set for reading.
         */
        void clear_socket_set_for_reading();

        /**
         * Adds a socket set for writing.
         * @param set set to add.
         */
        void add_socket_set_for_writing(const socket_set& set);

        /**
         * Removes a socket set for writing.
         * @param set set to remove.
         */
        void remove_socket_set_for_writing(const socket_set& set);

        /**
         * Empties the socket set for writing.
         */
        void clear_socket_set_for_writing();

        /**
         * Adds a socket set for error_checking.
         * @param set set to add.
         */
        void add_socket_set_for_error_checking(const socket_set& set);

        /**
         * Removes a socket set for error_checking.
         * @param set set to remove.
         */
        void remove_socket_set_for_error_checking(const socket_set& set);

        /**
         * Empties the socket set for error checking.
         */
        void clear_socket_set_for_error_checking();

        /**
         * Waits for any socket to be ready.
         * @param read_sockets sockets ready for reading.
         * @param write_sockets sockets written.
         * @param except_sockets sockets with errors.
         * @exception socket_error thrown when there is an error.
         */
        void wait(std::vector<socket*> &read_sockets, std::vector<socket*>& write_sockets, std::vector<socket*>& except_sockets);

        /**
         * Waits for any socket to be ready, with a timeout.
         * @param read_sockets sockets ready for reading.
         * @param write_sockets sockets written.
         * @param except_sockets sockets with errors.
         * @exception socket_error thrown when there is an error.
         * @return true if there was no timeout, false if there was a timeout.
         */
        bool wait(std::vector<socket*>& read_sockets, std::vector<socket*>& write_sockets, std::vector<socket*>& except_sockets, const std::chrono::microseconds& timeout);

    private:
        mutable std::shared_mutex m_mutex;
        socket_set m_read_set;
        socket_set m_write_set;
        socket_set m_except_set;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_SELECTOR_HPP
