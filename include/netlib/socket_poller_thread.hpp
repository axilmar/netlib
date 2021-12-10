#ifndef NETLIB_SOCKET_POLLER_THREAD_HPP
#define NETLIB_SOCKET_POLLER_THREAD_HPP


#include <thread>
#include "socket_poller.hpp"


namespace netlib {


    /**
     * A socket poller thread.
     */
    class socket_poller_thread : public socket_poller {
    public:
        /**
         * The constructor.
         * Creates a thread which does socket polling in a background thread.
         * @param max_sockets max sockets for the socket poller.
         */
        socket_poller_thread(size_t max_sockets = socket_poller::max_sockets);

        /**
         * Stops the socket poller thread and waits for its termination.
         */
        ~socket_poller_thread();

        /**
         * Wait for socket poller thread to terminate.
         * Also called from the destructor.
         */
        void stop();

        /**
         * do not wait for socket poller thread to terminate.
         */
        void detach() {
            m_thread.detach();
        }

    private:
        //the thread
        std::thread m_thread;

        //the thread function
        void run();
    };


} //namespace netlib


#endif //NETLIB_SOCKET_POLLER_THREAD_HPP
