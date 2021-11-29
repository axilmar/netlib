#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <utility>
#include "address_family.hpp"
#include "socket_type.hpp"
#include "protocol.hpp"
#include "socket_address.hpp"


namespace netlib {


    /**
     * A socket.
     */
    class socket {
    public:
        /**
         * The default constructor.
         * The socket is initialized in an invalid state.
         */
        socket();

        /**
         * Constructor from address family, socket type and protocol type.
         * @param af address family.
         * @param type socket type.
         * @param protocol protocol.
         * @exception std::invalid_argument thrown if the socket cannot be created.
         */
        socket(int af, int type, int protocol);

        /**
         * Constructor from address family, socket type and protocol type.
         * @param af address family.
         * @param st socket type.
         * @param p protocol.
         * @exception std::invalid_argument thrown if the socket cannot be created.
         */
        socket(address_family af, socket_type st, protocol p);

        /**
         * Constructor from address family and socket type.
         * The protocol is autodetected based on socket type.
         * @param af address family.
         * @param st socket type.
         * @exception std::invalid_argument thrown if the socket cannot be created.
         */
        socket(address_family af, socket_type st);

        /**
         * The class is not copyable.
         */
        socket(const socket&) = delete;

        /**
         * The move constructor.
         * @param src socket; on return, it becomes invalid.
         */
        socket(socket&& src);

        /**
         * Shuts down and closes the socket.
         */
        ~socket();

        /**
         * The class is not copyable.
         */
        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator.
         * @param src source; on return, it becomes invalid.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

        /**
         * Puts the socket in listening state.
         * @param max_connections maximum number of connections; if 0, then the system will select max number of connections.
         * @exception std::runtime_error thrown if there was an error.
         */
        void listen(int max_connections = 0);

        /**
         * Accepts a connection.
         * @return pair of created socket and socket address of the connecting entity.
         * @exception std::runtime_error thrown if there was an error.
         */
        std::pair<socket, socket_address> accept();

        /**
         * Binds the socket to the specific address.
         * @param address address.
         * @exception std::runtime_error thrown if there was an error.
         */
        void bind(const socket_address& address);

        /**
         * Connects the socket to the specific address.
         * @param address address.
         * @exception std::runtime_error thrown if there was an error.
         */
        void connect(const socket_address& address);

        /**
         * Sends data over the network.
         * @param buffer pointer to data to send.
         * @param size number of bytes to send.
         * @param flags send flags.
         * @return number of bytes sent, 0 if the connection is closed.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t send(const void* buffer, size_t size, int flags = 0);

        /**
         * Sends data over the network to a specific address.
         * @param buffer pointer to data to send.
         * @param size number of bytes to send.
         * @param address destination address.
         * @param flags send flags.
         * @return number of bytes sent, 0 if the connection is closed.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t send(const void* buffer, size_t size, const socket_address& address, int flags = 0);

        /**
         * Receives data from the network.
         * Blocking call.
         * @param buffer pointer to buffer to store the received data.
         * @param size number of bytes that can be stored in the buffer.
         * @param flags receive flags.
         * @return number of received bytes or 0 if the connection is closed.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t receive(void* buffer, size_t size, int flags = 0);

        /**
         * Receives data from the network, along with the address of the sender.
         * Blocking call.
         * @param buffer pointer to buffer to store the received data.
         * @param size number of bytes that can be stored in the buffer.
         * @param address the address of the sender.
         * @param flags receive flags.
         * @return number of received bytes or 0 if the connection is closed.
         * @exception std::runtime_error thrown if there was an error.
         */
        size_t receive(void* buffer, size_t size, socket_address& address, int flags = 0);

        /**
         * Checks if this socket is valid. 
         */
        explicit operator bool() const noexcept;

    private:
        uintptr_t m_handle;

        //internal constructor
        socket(uintptr_t handle) : m_handle(handle) {}
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP