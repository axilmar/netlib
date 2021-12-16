#include "platform.hpp"
#include <system_error>
#include "netlib/unencrypted_socket.hpp"


namespace netlib::unencrypted {


    //The default constructor.
    socket::socket() : m_handle(invalid_handle) {
    }


    //Constructor from handle.
    socket::socket(handle_type handle) : m_handle(handle) {
        if (handle < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
    }


    //Closes the underlying socket.
    socket::~socket() {
        closesocket(m_handle);
    }


    //Sets a new handle.
    void socket::set_handle(handle_type handle) {
        if (handle != m_handle) {
            closesocket(m_handle);
            m_handle = handle;
        }
    }


} //namespace netlib::unencrypted
