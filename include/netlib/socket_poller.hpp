#ifndef NETLIB_SOCKET_POLLER_HPP
#define NETLIB_SOCKET_POLLER_HPP


#include <functional>
#include <vector>
#include <mutex>
#include "ip4_udp_socket.hpp"


namespace netlib {


    /**
     * A class that can be used to poll multiple sockets at once. 
     * Thread-safe class.
     */
    class socket_poller {
    public:
        /**
         * event type. 
         */
        enum class event_type {
            /**
             * read event. 
             */
            read,

            /**
             * write event. 
             */
            write
        };

        /**
         * event callback type. 
         */
        using event_callback_type = std::function<void(socket_poller&, socket&, event_type)>;

        /**
         * poll status.
         */
        enum class poll_status {
            /**
             * stopped.
             */
            stopped = -2,

            /**
             * timeout.
             */
            timeout,

            /**
             * no sockets were polled. 
             */
            empty,

            /**
             * success.
             */
            success
        };

        /**
         * Constructor. 
         * @param max_sockets max number of entries.
         */
        socket_poller(size_t max_sockets = 1024);

        /**
         * The object is not copyable. 
         */
        socket_poller(const socket_poller&) = delete;

        /**
         * The object is not movable.
         */
        socket_poller(socket_poller&&) = delete;

        /**
         * Stops polling. 
         */
        ~socket_poller();

        /**
         * The object is not copyable.
         */
        socket_poller& operator = (const socket_poller&) = delete;

        /**
         * The object is not movable.
         */
        socket_poller& operator = (socket_poller&&) = delete;

        /**
         * Adds a socket entry.
         * @param s socket to add.
         * @param e event type.
         * @param cb callback type.
         * @return true if the entry is added, false if the poller is full.
         * @exception std::invalid_argument thrown if any of the parameters is invalid.
         */
        bool add(socket& s, event_type e, const event_callback_type& cb);

        /**
         * Removes a socket entry.
         * @param s socket to add.
         * @param e event type.
         * @exception std::invalid_argument thrown if any of the parameters is invalid.
         */
        void remove(socket& s, event_type e);

        /**
         * Removes all the entries for the given socket.
         * @param s socket.
         * @exception std::invalid_argument thrown if there is no entry for the given socket.
         */
        void remove(socket& s);

        /**
         * Polls all the added sockets.
         * It blocks until an event is reported.
         * It then calls the appropriate callback.
         * @param timeout_ms timeout, in milliseconds. If less than 0, then it blocks until there is an event.
         * @return poll_status poll status.
         * @exception std::runtime_error thrown if there was an error.
         */
        poll_status poll(int timeout_ms = -1);

    protected:
        /**
         * Invoked when a socket entry is added. 
         * @param entries_count number of entries after the addition.
         * @param s socket that was added.
         * @param e event type.
         * @param cb callback.
         */
        void on_socket_entry_added(const size_t entries_count, socket& s, event_type e, const event_callback_type& cb) {
        }

        /**
         * Invoked when a socket entry is removed.
         * @param entries_count number of entries after the removal.
         * @param s socket that was removed.
         * @param e event type.
         * @param cb callback.
         */
        void on_socket_entry_removed(const size_t entries_count, socket& s, event_type e, const event_callback_type& cb) {
        }

        /**
         * Invoked when a socket is removed.
         * @param entries_count number of entries after the removal.
         * @param s socket that was removed.
         */
        void on_socket_removed(const size_t entries_count, socket& s) {
        }

    private:
        //entry
        struct entry {
            socket* socket;
            event_type event;
            event_callback_type callback;
        };

        //mutex for synchronization
        mutable std::mutex m_mutex;

        //max sockets
        const size_t m_max_sockets;

        //internal socket used for waking up from poll.
        ip4::udp::socket m_com_socket;

        //socket address assigned to the com socket.
        ip4::socket_address m_com_socket_address;

        //entries
        std::vector<entry> m_entries;

        //if entries are changed
        bool m_entries_changed;

        //if polling should stop
        bool m_stop;

        //sets the entries as changed
        void set_entries_changed();
    };


} //namespace netlib


#endif //NETLIB_SOCKET_POLLER_HPP
