#include "netlib/socket_poller_thread.hpp"


namespace netlib {


    //Creates a thread which does socket polling in a background thread.
    socket_poller_thread::socket_poller_thread(size_t max_sockets) 
        : socket_poller(max_sockets)
        , m_thread(&socket_poller_thread::run, this)
    {
    }


    //Stops the socket poller thread and waits for its termination.
    socket_poller_thread::~socket_poller_thread() {
        stop();
    }


    /**
     * Wait for socket poller thread to terminate.
     */
    void socket_poller_thread::stop() {
        socket_poller::stop();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }


    //the thread function
    void socket_poller_thread::run() {
        for (;;) {
            if (poll() == poll_status::stopped) {
                return;
            }
        }
    }


} //namespace netlib
