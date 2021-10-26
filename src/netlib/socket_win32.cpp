#include <WinSock2.h>
#include <sstream>
#include "netlib/socket.hpp"


namespace netlib {


    //library initializer
    class library {
    public:
        //ensure library is initialized on first socket creation
        SOCKET create_socket(int af, int type, int protocol) {
            return ::socket(af, type, protocol);
        }

    private:
        //init library
        library() {
            WSADATA wsaData;
            const int error = WSAStartup(MAKEWORD(2, 0), &wsaData);
            if (error != NO_ERROR) {
                std::stringstream stream;
                stream << "Window Sockets library could not be initialized; error = ";
                stream << error;
                throw socket_error(stream.str());
            }
        }

        //destroy library
        ~library() {
            WSACleanup();
        }

        friend library& get_library();
    };


    //get library
    static library& get_library() {
        static library l;
        return l;
    }


    socket::socket(int af, int type, int protocol) 
        : m_handle(get_library().create_socket(af, type, protocol))
    {
    }


    socket::socket(socket&& src)
        : m_handle(src.m_handle)
    {
        src.m_handle = INVALID_SOCKET;
    }


    socket::~socket() {
        ::shutdown(m_handle, SD_BOTH);
        ::closesocket(m_handle);
    }


    socket& socket::operator = (socket&& src) {
        const uintptr_t temp = src.m_handle;
        src.m_handle = INVALID_SOCKET;
        m_handle = temp;
        return *this;
    }


    socket::operator bool() const {
        return m_handle == INVALID_SOCKET;
    }


    void socket::listen(int backlog) {
        if (::listen(m_handle, backlog == INT_MAX ? SOMAXCONN : backlog) == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Listen function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
    }


    std::pair<socket, socket_address> socket::accept() {
        std::pair<socket, socket_address> result{ socket{}, socket_address{} };
        int addr_size = sizeof(result.second.m_data);
        result.first = ::accept(m_handle, reinterpret_cast<sockaddr*>(result.second.m_data), &addr_size);
        if (result.first == INVALID_SOCKET) {
            std::stringstream stream;
            stream << "Accept function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
        return result;
    }


    void socket::connect(const socket_address& addr) {
        if (::connect(m_handle, reinterpret_cast<const sockaddr*>(addr.m_data), sizeof(addr.m_data)) == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Connect function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
    }


    void socket::bind(const socket_address& addr) {
        if (::bind(m_handle, reinterpret_cast<const sockaddr*>(addr.m_data), sizeof(addr.m_data)) == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Bind function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
    }


    size_t socket::send(const byte_buffer& buffer, int flags) {
        const int result = ::send(m_handle, reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()), flags);
        if (result == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Send function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
        return static_cast<size_t>(result);
    }


    size_t socket::send(const byte_buffer& buffer, const socket_address& addr, int flags) {
        const int result = ::sendto(m_handle, reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()), flags, reinterpret_cast<const sockaddr*>(addr.m_data), sizeof(addr.m_data));
        if (result == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Send-to function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
        return static_cast<size_t>(result);
    }


    size_t socket::receive(byte_buffer& buffer, int flags) {
        const int result = ::recv(m_handle, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags);
        if (result == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Receive function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
        return static_cast<size_t>(result);
    }


    size_t socket::receive(byte_buffer& buffer, socket_address& addr, int flags) {
        int addr_size = sizeof(addr.m_data);
        const int result = ::recvfrom(m_handle, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags, reinterpret_cast<sockaddr*>(addr.m_data), &addr_size);
        if (result == SOCKET_ERROR) {
            std::stringstream stream;
            stream << "Receive-from function failed; error: ";
            stream << WSAGetLastError();
            throw socket_error(stream.str());
        }
        return static_cast<size_t>(result);
    }


} //namespace netlib
