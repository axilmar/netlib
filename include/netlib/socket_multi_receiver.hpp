#ifndef NETLIB_SOCKET_MULTI_RECEIVER_HPP
#define NETLIB_SOCKET_MULTI_RECEIVER_HPP


#include <functional>
#include <chrono>
#include <mutex>
#include <map>
#include "socket.hpp"


namespace netlib {


    /**
     * Allows reception of messages using 'select'.
     * Thread-safe class.
     */
    class socket_multi_receiver {
    public:
        /**
         * Sets FD_SETSIZE to the given number of sockets.
         */
        static constexpr size_t MAX_SOCKETS = 1024;

        /**
         * Type of receive function to execute.
         */
        using receive_function = std::function<void(socket&)>;

        /**
         * The default constructor.
         */
        socket_multi_receiver();

        /**
         * Not copyable.
         */
        socket_multi_receiver(const socket_multi_receiver&) = delete;

        /**
         * Not movable.
         */
        socket_multi_receiver(socket_multi_receiver&&) = delete;

        /**
         * Not copyable.
         */
        socket_multi_receiver& operator = (const socket_multi_receiver&) = delete;

        /**
         * Not movable.
         */
        socket_multi_receiver& operator = (socket_multi_receiver&&) = delete;

        /**
         * Adds a receive function for the specified socket.
         * @param s socket.
         * @param f function to be invoked when data are ready for reception in the given socket.
         */
        void add(socket& s, receive_function&& f);

        /**
         * Adds a receive function for the specified socket.
         * @param s socket.
         * @param f function to be invoked when data are ready for reception in the given socket.
         * @exception std::invalid_argument thrown if the socket is already added to this.
         */
        template <class F> void add(socket& s, F&& f) {
            add(s, receive_function(f));
        }

        /**
         * Removes a socket.
         * @param s socket to remove.
         * @exception std::invalid_argument thrown if the socket has not been added to this.
         */
        void remove(socket& s);

        /**
         * Receives data.
         * It blocks the execution of the current thread.
         * It invokes the appropriate receive function, depending on the sockets that had data.
         * @exception socket_error thrown if there is a socket error.
         */
        void receive();

        /**
         * Receives data.
         * It blocks the execution of the current thread until the given time passes.
         * It invokes the appropriate receive function, depending on the sockets that had data.
         * @param timeout timeout.
         * @return true if data were received, false if there was a timeout.
         * @exception socket_error thrown if there is a socket error.
         */
        bool receive(const std::chrono::microseconds& timeout);

        /**
         * Same as receive_data(microseconds).
         * @param timeout timeout; converted to microseconds by std::chrono::duration_cast<std::chrono::microseconds>.
         */
        template <class T> bool receive(const T& timeout) {
            return receive(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
        }

    private:
        std::mutex m_mutex;
        std::map<uintptr_t, std::pair<socket*, receive_function>> m_receive_functions;
        std::vector<byte> m_fd_set;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_MULTI_RECEIVER_HPP
