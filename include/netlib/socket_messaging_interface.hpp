#ifndef NETLIB_SOCKET_MESSAGING_INTERFACE_HPP
#define NETLIB_SOCKET_MESSAGING_INTERFACE_HPP


#include "messaging_interface.hpp"
#include "socket.hpp"
#include "socket_selector.hpp"


namespace netlib {


    /**
     * Base class for messaging interfaces that use sockets.
     */
    class socket_messaging_interface : public messaging_interface {
    public:
        /**
         * Checks if the socket is open.
         * @return true if the socket is open, false otherwise.
         */
        bool is_socket_open() const;

        /**
         * Shuts down the socket.
         * @param shutdown_send removes pending data.
         * @param shutdown_receive stops listening to this socket.
         */
        void shutdown_socket(bool shutdown_send = true, bool shutdown_receive = true);

        /**
         * Closes the socket.
         */
        void close_socket();

        /**
         * Returns a socket option.
         * @param level socket level.
         * @param option_id option id.
         * @param option_value buffer with option data.
         * @exception socket_error thrown if the operation was invalid.
         */
        void get_socket_option(int level, int option_id, byte_buffer& option_value) const;

        /**
         * Sets a socket option.
         * @param level socket level.
         * @param option_id option id.
         * @param option_value buffer with option data.
         * @exception socket_error thrown if the operation was invalid.
         */
        void set_socket_option(int level, int option_id, const byte_buffer& option_value);

        /**
         * Returns a socket option; typed interface.
         * @param level socket level.
         * @param option_id option id.
         * @return socket option value; must be a pod object.
         * @exception socket_error thrown if the operation was invalid.
         */
        template <class T, typename = std::enable_if_t<std::is_pod_v<T>>>
        T get_socket_option(int level, int option_id) const {
            return m_socket.get_option<T>(level, option_id);
        }

        /**
         * Sets a socket option; typed interface.
         * @param level socket level.
         * @param option_id option id.
         * @param value value to set; must be a pod object.
         * @exception socket_error thrown if the operation was invalid.
         */
        template <class T, typename = std::enable_if_t<std::is_pod_v<T>>>
        void set_socket_option(int level, int option_id, const T& value) {
            return m_socket.set_option(level, option_id, value);
        }

        /**
         * Returns the reuse option.
         */
        bool get_socket_reuse() const;

        /**
         * Sets the reuse option.
         */
        void set_socket_reuse(bool v);

        /**
         * Connects the socket to the specified address.
         * @param addr address to connect to.
         * @exception socket_error thrown if the operation was invalid.
         */
        void connect_socket(const socket_address& addr);

        /**
         * Binds the socket to the specified address.
         * @param addr address to bind the socket to.
         * @exception socket_error thrown if the operation was invalid.
         */
        void bind_socket(const socket_address& addr);

        using receiving_messaging_interface::receive_message;

        /**
         * Adds the socket of this interface to the given socket set.
         * @param set socket set to add this to.
         */
        void add_to_socket_set(socket_set& set) { set.add(m_socket); }

        /**
         * Removes the socket of this interface from the given socket set.
         * @param set socket set to remove this from.
         */
        void remove_from_socket_set(socket_set& set) { set.remove(m_socket); }

        /**
         * Adds the socket of this interface to the given socket selector for reading.
         * @param selector selector to add this to.
         */
        void add_to_socket_selector_for_reading(socket_selector& selector) { selector.add_socket_for_writing(m_socket); }

        /**
         * Removes the socket of this interface to the given socket selector for reading.
         * @param selector selector to add this to.
         */
        void remove_from_socket_selector_for_reading(socket_selector& selector) { selector.remove_socket_for_reading(m_socket); }

        /**
         * Adds the socket of this interface to the given socket selector for writing.
         * @param selector selector to add this to.
         */
        void add_to_socket_selector_for_writing(socket_selector& selector) { selector.add_socket_for_writing(m_socket); }

        /**
         * Removes the socket of this interface to the given socket selector for writing.
         * @param selector selector to add this to.
         */
        void remove_from_socket_selector_for_writing(socket_selector& selector) { selector.remove_socket_for_writing(m_socket); }

        /**
         * Adds the socket of this interface to the given socket selector for error_checking.
         * @param selector selector to add this to.
         */
        void add_to_socket_selector_for_error_checking(socket_selector& selector) { selector.add_socket_for_error_checking(m_socket); }

        /**
         * Removes the socket of this interface to the given socket selector for error_checking.
         * @param selector selector to add this to.
         */
        void remove_from_socket_selector_for_error_checking(socket_selector& selector) { selector.remove_socket_for_error_checking(m_socket); }

    protected:
        /**
         * The default constructor.
         */
        socket_messaging_interface() {}

        /**
         * Constructor from address family, socket type and protocol type.
         * @param af address family.
         * @param type socket type.
         * @param protocol protocol type.
         * @exception socket_error thrown if the socket could not be created.
         */
        socket_messaging_interface(int af, int type, int protocol);

        /**
         * Constructor from socket type.
         * @param type socket type.
         * @exception socket_error thrown if the socket could not be created.
         */
        socket_messaging_interface(socket::TYPE type);

        /**
         * Constructor from socket.
         * @param socket socket.
         */
        socket_messaging_interface(class socket&& socket);

        /**
         * Returns the socket.
         * @return the socket.
         */
        const socket& get_socket() const { return m_socket; }

        /**
         * Returns the socket.
         * @return the socket.
         */
        socket& get_socket() { return m_socket; }

        /**
         * Sets the socket.
         * @param socket socket.
         */
        void set_socket(class socket&& socket);

        /**
         * Moves a socket, and checks it if it is of the given type.
         * @param socket socket to move.
         * @param socket_type socket type.
         * @exception socket_error if the socket type is not appropriate.
         */
        static socket&& move_socket(socket& socket, int socket_type);

    private:
        //the socket.
        socket m_socket;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_MESSAGING_INTERFACE_HPP
