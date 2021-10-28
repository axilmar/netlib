#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <cstdint>
#include <utility>
#include <type_traits>
#include "socket_error.hpp"
#include "socket_address.hpp"
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * A socket.
     */
    class socket : public constants {
    public:
        /**
         * socket stream type.
         */
        static const int SOCKET_STREAM;

        /**
         * socket datagram type.
         */
        static const int SOCKET_DATAGRAM;

        /**
         * TCP protocol type.
         */
        static const int IP_PROTOCOL_TCP;

        /**
         * UDP protocol type.
         */
        static const int IP_PROTOCOL_UDP;

        /**
         * socket level options.
         */
        static const int LEVEL_SOCKET;

        /**
         * TCP level options.
         */
        static const int LEVEL_TCP;

        /**
         * Reuse address option.
         */
        static const int OPTION_REUSE_ADDRESS;

        /**
         * Disables the algorithm that sends the data only when enough data have been put to the queue.
         */
        static const int OPTION_DISABLE_TCP_SEND_COALESCING;

        /**
         * Socket type.
         */
        enum class TYPE {
            /**
             * TCP socket for IP4.
             */
            TCP_IP4,

            /**
             * TCP socket for IP6.
             */
            TCP_IP6,

            /**
             * UDP socket for IP4.
             */
            UDP_IP4,

            /**
             * UDP socket for IP6.
             */
            UDP_IP6
        };

        /**
         * Creates a closed socket.
         */
        socket();

        /**
         * Creates a socket.
         * @param af address family.
         * @param type socket type.
         * @param protocol protocol type.
         * @exception socket_error thrown if the socket could not be created.
         */
        socket(int af, int type, int protocol);

        /**
         * Creates a socket.
         * @param type socket type.
         * @exception socket_error thrown if the socket could not be created.
         */
        socket(TYPE type);

        socket(const socket&) = delete;

        /**
         * The move constructor.
         * @param src source object.
         */
        socket(socket&& src);

        /**
         * Shuts down and closes the socket.
         */
        ~socket();

        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

        /**
         * Checks if the socket has a valid socket handle.
         * @return true if the socket is valid, false otherwise.
         */
        operator bool() const;

        /**
         * Opens a socket.
         * @param af address family.
         * @param type socket type.
         * @param protocol protocol type.
         * @exception socket_error thrown if the socket could not be created.
         */
        void open(int af, int type, int protocol);

        /**
         * Opens a socket.
         * @param type socket type.
         * @exception socket_error thrown if the socket could not be created.
         */
        void open(TYPE type);

        /**
         * Shuts down this socket.
         * @param shutdown_send removes pending data.
         * @param shutdown_receive stops listening to this socket.
         */
        void shutdown(bool shutdown_send = true, bool shutdown_receive = true);

        /**
         * Closes the socket.
         */
        void close();

        /**
         * Returns a socket option.
         * @param level socket level.
         * @param option_id option id.
         * @param option_value buffer with option data.
         * @exception socket_error thrown if the operation was invalid.
         */
        void get_option(int level, int option_id, byte_buffer& option_value) const;

        /**
         * Sets a socket option.
         * @param level socket level.
         * @param option_id option id.
         * @param option_value buffer with option data.
         * @exception socket_error thrown if the operation was invalid.
         */
        void set_option(int level, int option_id, const byte_buffer& option_value);

        /**
         * Returns a socket option; typed interface.
         * @param level socket level.
         * @param option_id option id.
         * @return socket option value; must be a pod object.
         * @exception socket_error thrown if the operation was invalid.
         */
        template <class T, typename = std::enable_if_t<std::is_pod_v<T>>> 
        T get_option(int level, int option_id) const {
            byte_buffer buffer{ sizeof(T) };
            get_option(level, option_id, buffer);
            return *reinterpret_cast<const T*>(buffer.data());
        }

        /**
         * Sets a socket option; typed interface.
         * @param level socket level.
         * @param option_id option id.
         * @param value value to set; must be a pod object.
         * @exception socket_error thrown if the operation was invalid.
         */
        template <class T, typename = std::enable_if_t<std::is_pod_v<T>>> 
        void set_option(int level, int option_id, const T& value) {
            byte_buffer buffer{ sizeof(T) };
            *reinterpret_cast<T*>(buffer.data()) = value;
            set_option(level, option_id, buffer);
        }

        /**
         * Returns the reuse option.
         */
        bool get_reuse() const;

        /**
         * Sets the reuse option.
         */
        void set_reuse(bool v);

        /**
         * Returns the disable tcp send coalescing option.
         */
        bool get_disable_tcp_send_coalescing() const;

        /**
         * Sets the disable tcp send coalescing option.
         */
        void set_disable_tcp_send_coalescing(bool v);

        /**
         * Listens to a socket.
         * @param backlog backlog.
         * @exception socket_error thrown if the operation was invalid.
         */
        void listen(int backlog = INT_MAX);
        
        /**
         * Waits for a socket connection.
         * @return pair of socket that can be used to send/receive data, and socket address.
         * @exception socket_error thrown if the operation was invalid.
         */
        std::pair<socket, socket_address> accept();

        /**
         * Connects to the specified address.
         * @param addr address to connect to.
         * @exception socket_error thrown if the operation was invalid.
         */
        void connect(const socket_address& addr);

        /**
         * Binds the socket to the specified address.
         * @param addr address to bind the socket to.
         * @exception socket_error thrown if the operation was invalid.
         */
        void bind(const socket_address& addr);

        /**
         * Sends data.
         * @param buffer buffer with data to send.
         * @param size number of bytes in the buffer.
         * @param flags send flags.
         * @return number of bytes sent.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t send(const void* buffer, size_t size, int flags = 0);

        /**
         * Sends data.
         * @param buffer buffer with data to send.
         * @param flags send flags.
         * @return number of bytes sent.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t send(const byte_buffer& buffer, int flags = 0);

        /**
         * Sends data to a specific address.
         * @param buffer buffer with data to send.
         * @param addr destination address.
         * @param flags send flags.
         * @return number of bytes sent.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t send(const byte_buffer& buffer, const socket_address& addr, int flags = 0);

        /**
         * Receives data.
         * @param buffer buffer with data to receive.
         * @param size size of buffer.
         * @param flags receive flags.
         * @return number of bytes received.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t receive(void* buffer, size_t size, int flags = 0);

        /**
         * Receives data.
         * @param buffer buffer with data to receive.
         * @param flags receive flags.
         * @return number of bytes received.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t receive(byte_buffer& buffer, int flags = 0);

        /**
         * Receives data from a specific address.
         * @param buffer buffer with data to receive.
         * @param addr source address.
         * @param flags receive flags.
         * @return number of bytes received.
         * @exception socket_error thrown if the operation was invalid.
         */
        size_t receive(byte_buffer& buffer, socket_address& addr, int flags = 0);

        /**
         * Returns true if this socket is a streaming socket, false otherwise. 
         */
        bool is_streaming_socket() const { return m_streaming_socket; }

        /**
         * Does not return until all the data are sent.
         * @param buffer pointer to data to send.
         * @param size number of bytes to send.
         * @param flaga send flags.
         * @return true if the data were successfully sent, false if the socket was closed.
         */
        bool stream_send(const void* buffer, size_t size, int flags = 0);

        /**
         * Does not return until all the data are received.
         * @param buffer pointer to data to receive.
         * @param size number of bytes to receive.
         * @param flaga send flags.
         * @return true if the data were successfully received, false if the socket was closed.
         */
        bool stream_receive(void* buffer, size_t size, int flags = 0);

    private:
        //handle
        uintptr_t m_handle;

        //if it is a streaming socket
        bool m_streaming_socket{};

        //internal constructor
        socket(uintptr_t handle) : m_handle(handle) {}
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
