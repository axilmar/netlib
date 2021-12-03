#include "netlib/platform.hpp"
#include <stdexcept>
#include <limits>
#include "netlib/socket.hpp"


namespace netlib {


    #ifdef _WIN32
    static constexpr uintptr_t invalid_socket_handle = INVALID_SOCKET;
    #endif


    #ifdef linux
    static constexpr uintptr_t invalid_socket_handle = -1;
    #endif


    //get protocol for socket type
    static protocol protocol_from_socket_type(socket_type st) {
        switch (st) {
        case socket_type::stream:
            return protocol::tcp;

        case socket_type::datagram:
            return protocol::udp;
        }

        throw std::logic_error("Unsupported socket type.");
    }


    //checks value stored in size_t against int
    static void check_buffer_size(size_t size) {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max())) {
            throw std::invalid_argument("Given size value too large for the sockets API.");
        }
    }


    //checks if the socket is still connected.
    static bool is_socket_connected(uintptr_t handle) {
        //get option
        int optval;
        socklen_t optlen = sizeof(optval);
        int res = getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&optval), &optlen);

        //handle result
        if (res) {
            //handle result on win32
            #ifdef _WIN32
            DWORD error = WSAGetLastError();
            switch (error) {
            case WSAENOTSOCK:
                return false;
            default:
                throw std::runtime_error(get_last_error_message(error));
            }
            #endif

            //handle result on linux
            #ifdef linux
            switch (errno) {
            case EBADF:
            case ENOTSOCK:
                return false;
            default:
                throw std::runtime_error(get_last_error_message(errno));
            }
            #endif
        }

        //handle error value in win32
        #ifdef WIN32
        switch (optval) {
        case WSAENOTCONN:
        case WSAENETRESET:
        case WSAENOTSOCK:
        case WSAESHUTDOWN:
        case WSAECONNABORTED:
        case WSAETIMEDOUT:
        case WSAECONNRESET:
            return false;
        }
        #endif

        //handle error value in linux
        #ifdef linux
        switch (optval) {
        case ECONNRESET:
        case EBADF:
        case ENOTCONN:
        case ENOTSOCK:
        case EPIPE:
            return false;
        }
        #endif

        //socket still connected
        return true;
    }


    //handle send result
    static size_t handle_send_result(uintptr_t &handle, int bytes_sent) {
        //normal case
        if (bytes_sent >= 0) {
            return bytes_sent;
        }

        //if not connected, reset the handle and return not-size.
        if (!is_socket_connected(handle)) {
            handle = invalid_socket_handle;
            return socket::nsize;
        }

        throw std::runtime_error(get_last_error_message());
    }


    //handle receive result
    static size_t handle_receive_result(uintptr_t& handle, int bytes_received) {
        //normal case; some data were received
        if (bytes_received > 0) {
            return bytes_received;
        }

        //special case; need to detect if the connection is closed
        if (bytes_received == 0) {
            //if the socket is still connected, then 0 is allowed
            if (is_socket_connected(handle)) {
                return bytes_received;
            }

            //the connection was broken; reset the handle and return no-size.
            handle = invalid_socket_handle;
            return socket::nsize;
        }

        //error
        handle = invalid_socket_handle;
        throw std::runtime_error(get_last_error_message());
    }


    //invalid socket constructor.
    socket::socket() 
        : m_handle{ invalid_socket_handle }
    {
    }


    //open socket
    socket::socket(int af, int type, int protocol)
        : m_handle{ ::socket(af, type, protocol) }
    {
        if (m_handle == invalid_socket_handle) {
            throw std::runtime_error(get_last_error_message());
        }
    }


    //open socket from enumerated values
    socket::socket(address_family af, socket_type st, protocol p)
        : socket(address_family_to_system_value(af), socket_type_to_system_value(st), protocol_to_system_value(p))
    {
    }


    //detect protocol from socket type
    socket::socket(address_family af, socket_type st)
        : socket(af, st, protocol_from_socket_type(st))
    {
    }


    //move constructor
    socket::socket(socket&& src)
        : m_handle(src.m_handle)
    {
        src.m_handle = invalid_socket_handle;
    }


    //close the socket
    socket::~socket() {
        if (m_handle != invalid_socket_handle) {
            shutdown(m_handle, SD_BOTH);
            closesocket(m_handle);
        }
    }


    //move assignment
    socket& socket::operator = (socket&& src) {
        uintptr_t temp = src.m_handle;
        src.m_handle = invalid_socket_handle;
        m_handle = temp;
        return *this;
    }


    //listen
    void socket::listen(int max_connections) {
        int error = ::listen(m_handle, max_connections = 0 ? SOMAXCONN : max_connections);

        if (!error) {
            return;
        }

        throw std::runtime_error(get_last_error_message());
    }


    //accept
    std::pair<socket, socket_address> socket::accept() {
        socket_address addr;
        auto new_socket_handle = ::accept(m_handle, reinterpret_cast<sockaddr*>(addr.data()), nullptr);

        if (new_socket_handle != invalid_socket_handle) {
            return { socket(new_socket_handle), addr };
        }

        throw std::runtime_error(get_last_error_message());
    }


    //bind
    void socket::bind(const socket_address& address) {
        int error = ::bind(m_handle, reinterpret_cast<const sockaddr*>(address.data()), socket_address::data_size);

        if (!error) {
            return;
        }

        throw std::runtime_error(get_last_error_message());
    }


    //connect
    void socket::connect(const socket_address& address) {
        int error = ::connect(m_handle, reinterpret_cast<const sockaddr*>(address.data()), socket_address::data_size);

        if (!error) {
            return;
        }

        throw std::runtime_error(get_last_error_message());
    }


    //send
    size_t socket::send(const void* buffer, size_t size, int flags) {
        check_buffer_size(size);
        int bytes_sent = ::send(m_handle, reinterpret_cast<const char*>(buffer), static_cast<int>(size), flags);
        return handle_send_result(m_handle, bytes_sent);
    }


    //send-to
    size_t socket::send(const void* buffer, size_t size, const socket_address& addr, int flags) {
        check_buffer_size(size);
        int bytes_sent = ::sendto(m_handle, reinterpret_cast<const char*>(buffer), static_cast<int>(size), flags, reinterpret_cast<const sockaddr*>(addr.data()), socket_address::data_size);
        return handle_send_result(m_handle, bytes_sent);
    }


    //receive
    size_t socket::receive(void* buffer, size_t size, int flags) {
        check_buffer_size(size);
        int bytes_received = recv(m_handle, reinterpret_cast<char*>(buffer), static_cast<int>(size), flags);
        return handle_receive_result(m_handle, bytes_received);
    }


    //receive from
    size_t socket::receive(void* buffer, size_t size, socket_address& addr, int flags) {
        check_buffer_size(size);
        int addr_len = socket_address::data_size;
        int bytes_received = recvfrom(m_handle, reinterpret_cast<char*>(buffer), static_cast<int>(size), flags, reinterpret_cast<sockaddr*>(addr.data()), &addr_len);
        return handle_receive_result(m_handle, bytes_received);
    }


    //Checks if this socket is valid.
    socket::operator bool() const noexcept {
        return m_handle != invalid_socket_handle;
    }


    //Returns true if the socket is stream-oriented, false otherwise.
    bool socket::is_stream_oriented() const {
        //get option
        int optval;
        int optlen = sizeof(optval);
        int error = getsockopt(m_handle, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&optval), &optlen);

        //if no error, check if the socket is stream
        if (!error) {
            return optval == SOCK_STREAM ? true : false;
        }

        //error
        throw std::runtime_error(get_last_error_message());
    }


    //Receives data from the socket.
    io_resource::io_result_type socket::read(void* buffer, size_t size) {
        const size_t res = receive(buffer, size);
        return {res, res != nsize};
    }


    //Sends data over the socket.
    io_resource::io_result_type socket::write(const void* buffer, size_t size) {
        const size_t res = send(buffer, size);
        return { res, res != nsize };
    }


} //namespace netlib
