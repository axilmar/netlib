#include "netlib/socket_messaging_interface.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/serialization.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/socket_error.hpp"


namespace netlib {


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
