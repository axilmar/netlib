#include "netlib/socket_messaging_interface.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/serialization.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/socket_error.hpp"


namespace netlib {


    //internal buffer for the thread
    static thread_local byte_buffer thread_buffer;


    //check if open
    bool socket_messaging_interface::is_socket_open() const {
        return static_cast<bool>(m_socket);
    }


    //shutdown socket
    void socket_messaging_interface::shutdown_socket(bool shutdown_send, bool shutdown_receive) {
        m_socket.shutdown(shutdown_send, shutdown_receive);
    }


    //close socket
    void socket_messaging_interface::close_socket() {
        m_socket.close();
    }


    //get socket option
    void socket_messaging_interface::get_socket_option(int level, int option_id, byte_buffer& option_value) const {
        m_socket.get_option(level, option_id, option_value);
    }


    //set socket option
    void socket_messaging_interface::set_socket_option(int level, int option_id, const byte_buffer& option_value) {
        m_socket.set_option(level, option_id, option_value);
    }


    //get socket reuse
    bool socket_messaging_interface::get_socket_reuse() const {
        return m_socket.get_reuse();
    }


    //set socket reuse
    void socket_messaging_interface::set_socket_reuse(bool v) {
        m_socket.set_reuse(v);
    }


    //connect the socket
    void socket_messaging_interface::connect_socket(const socket_address& addr) {
        m_socket.connect(addr);
    }


    //bind the socket
    void socket_messaging_interface::bind_socket(const socket_address& addr) {
        m_socket.bind(addr);
    }


    //Sends a message.
    bool socket_messaging_interface::send_message(const message& msg) {
        thread_buffer.clear();
        msg.serialize(thread_buffer);
        return send_data(thread_buffer);
    }


    //receives a message.
    message_pointer socket_messaging_interface::receive_message(std::pmr::memory_resource& memres, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_data(thread_buffer)) {
            return message_pointer{ nullptr, message_deleter(memres, 0) };
        }

        //peek the message id in order to create the appropriate message from the received id
        message_id id;
        copy_value(&id, reinterpret_cast<const message_id&>(thread_buffer[0]));

        //create a message from the id
        message_pointer result = message_registry::create_message(id, memres);

        //deserialize the message
        byte_buffer::position pos{ 0 };
        result->deserialize(thread_buffer, pos);

        //return the message
        return result;
    }


    //closed socket constructor
    socket_messaging_interface::socket_messaging_interface() {
    }


    //constructor from parameters
    socket_messaging_interface::socket_messaging_interface(int af, int type, int protocol)
        : m_socket(af, type, protocol)
    {
    }


    //constructor from type
    socket_messaging_interface::socket_messaging_interface(socket::TYPE type)
        : m_socket(type)
    {
    }


    //Constructor from socket.
    socket_messaging_interface::socket_messaging_interface(class socket&& socket)
        : m_socket(std::move(socket))
    {
    }


    //Returns the socket.
    const socket& socket_messaging_interface::get_socket() const {
        return m_socket;
    }


    //Returns the socket.
    socket& socket_messaging_interface::get_socket() {
        return m_socket;
    }


    //Sets the socket.
    void socket_messaging_interface::set_socket(class socket&& socket) {
        m_socket = std::move(socket);
    }


    //Moves a socket, and checks it if it is of the given type.
    socket&& socket_messaging_interface::move_socket(socket& socket, int socket_type) {
        if (socket.get_type() != socket_type) {
            throw socket_error(stringstream() << "different socket type; expected socket type = " << socket::get_socket_type_name(socket_type) << "; actual socket type = " << socket::get_socket_type_name(socket.get_type()));
        }
        return std::move(socket);
    }


} //namespace netlib
