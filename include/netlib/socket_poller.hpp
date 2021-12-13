#ifndef NETLIB_SOCKET_POLLER_HPP
#define NETLIB_SOCKET_POLLER_HPP


#include <functional>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include "udp_server_socket.hpp"


/**
 * Socket poller max sockets preprocessor definition.
 */
#ifndef NETLIB_SOCKET_POLLER_MAX_SOCKETS
#define NETLIB_SOCKET_POLLER_MAX_SOCKETS 1024
#endif


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
         * Status flags. 
         */
        struct status_flags {
            /**
             * An error happened. 
             */
            bool error : 1;

            /**
             * A connection was aborted (for connection-oriented sockets). 
             */
            bool connection_aborted : 1;

            /**
             * Invalid socket. 
             */
            bool invalid_socket : 1;
        };

        using socket_ptr = std::shared_ptr<socket>;

        /**
         * event callback type. 
         */
        using event_callback_type = std::function<void(socket_poller&, const socket_ptr&, event_type, status_flags)>;

        /**
         * poll status.
         */
        enum class poll_status {
            /**
             * stopped.
             */
            stopped = -2,

            /**
             * no sockets were polled.
             */
             empty,

             /**
             * timeout.
             */
            timeout,

            /**
             * success.
             */
            success
        };

        /**
         * max socket limit.
         */
        static constexpr size_t max_sockets = NETLIB_SOCKET_POLLER_MAX_SOCKETS;

        /**
         * Constructor. 
         * @param max_sockets max number of entries.
         */
        socket_poller(size_t max_sockets = socket_poller::max_sockets);

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
        bool add(const socket_ptr& s, event_type e, const event_callback_type& cb);

        /**
         * Adds a socket for reading.
         * @param s socket to add.
         * @param cb callback.
         * @return true if the entry is added, false if the poller is full.
         * @exception std::invalid_argument thrown if any of the parameters is invalid.
         */
        bool add(const socket_ptr& s, const event_callback_type& cb) {
            return add(s, event_type::read, cb);
        }

        /**
         * Adds a socket for reading.
         * @param s socket to add.
         * @param cb callback lambda; the socket type can be anything derived from class socket.
         * @return true if the entry is added, false if the poller is full.
         * @exception std::invalid_argument thrown if any of the parameters is invalid.
         */
        template <class S, class F> bool add(const std::shared_ptr<S>& s, const F& cb) {
            return add(std::static_pointer_cast<socket>(s), event_callback_type([cb](socket_poller& sp, const socket_ptr& s, event_type e, status_flags f) {
                return cb(sp, std::static_pointer_cast<S>(s), e, f);
                }));
        }

        /**
         * Removes a socket entry.
         * @param s socket to add.
         * @param e event type.
         * @exception std::invalid_argument thrown if any of the parameters is invalid.
         */
        void remove(const socket_ptr& s, event_type e);

        /**
         * Removes all the entries for the given socket.
         * @param s socket.
         * @exception std::invalid_argument thrown if there is no entry for the given socket.
         */
        void remove(const socket_ptr& s);

        /**
         * Polls all the added sockets.
         * It blocks until an event is reported.
         * It then calls the appropriate callback.
         * @param timeout_ms timeout, in milliseconds. If less than 0, then it blocks until there is an event.
         * @return poll_status poll status.
         * @exception std::runtime_error thrown if there was an error.
         * @exception std::logic_error thrown if more than one thread attempts to poll.
         */
        poll_status poll(int timeout_ms = -1);

        /**
         * Sets the callback that is invoked when a socket entry is added.
         * @param f function to invoke for this callback.
         */
        void set_on_socket_entry_added_callback(const std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)>& f);

        /**
         * Sets the callback that is invoked when a socket entry is removed.
         * @param f function to invoke for this callback.
         */
        void set_on_socket_entry_remmoved_callback(const std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)>& f);

        /**
         * Sets the callback that is invoked when a socket is removed.
         * @param f function to invoke for this callback.
         */
        void set_on_socket_removed_callback(const std::function<void(const size_t entries_count, const socket_ptr& s)>& f);

        /**
         * Stops the socket poller, if not stopped yet.
         * Also invoked in the destructor.
         */
        void stop();

    private:
        //entry
        struct entry {
            socket_ptr socket;
            event_type event;
            event_callback_type callback;
        };

        //mutex for synchronization
        mutable std::mutex m_mutex;

        //max sockets
        const size_t m_max_sockets;

        //internal socket used for waking up from poll.
        std::shared_ptr<udp::server_socket> m_com_socket;

        //socket address assigned to the com socket.
        socket_address m_com_socket_address;

        //entries
        std::vector<entry> m_entries;

        //if entries are changed
        bool m_entries_changed;

        //if polling should stop
        bool m_stop;

        //sets the entries as changed
        void set_entries_changed();

        //callbacks
        std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)> m_on_socket_entry_added;
        std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)> m_on_socket_entry_removed;
        std::function<void(const size_t entries_count, const socket_ptr& s)> m_on_socket_removed;

        //data used for polling
        std::vector<entry> m_poll_entries;
        std::vector<struct pollfd> m_poll_fds;

        //used for detecting multithreaded poll attempts
        std::atomic<size_t> m_poll_counter;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_POLLER_HPP
