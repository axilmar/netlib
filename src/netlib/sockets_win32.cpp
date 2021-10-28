#include <WinSock2.h>
#include <In6addr.h>
#include <Ws2tcpip.h>
#include <sstream>
#include "netlib/socket.hpp"


namespace netlib {


    /**************************************************************************
        internals
     **************************************************************************/


     //library initializer
    class library {
    public:
        //ensure library is initialized on first socket creation
        SOCKET create_socket(int af, int type, int protocol) {
            return ::socket(af, type, protocol);
        }

        //return size of network_address buffer
        static constexpr size_t get_address_buffer_size() {
            return sizeof(network_address::m_data);
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


    //get string with last error message
    static std::string get_last_error(DWORD error_id = 0) {
        //get the error message
        if (!error_id) {
            error_id = WSAGetLastError();
        }

        //no error
        if (error_id == 0) {
            return std::string();
        }

        LPSTR buffer = nullptr;

        //get error message as string
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);

        //copy it to an std::string
        std::string msg_str(buffer, size);

        //free the buffer
        LocalFree(buffer);

        //create the message
        std::stringstream stream;
        stream << '[' << error_id << "] " << msg_str;
        return stream.str();
    }


    /**************************************************************************
        class constants
     **************************************************************************/


    //constants
    const int constants::ADDRESS_FAMILY_IP4 = AF_INET;
    const int constants::ADDRESS_FAMILY_IP6 = AF_INET6;


    /**************************************************************************
        class network_address
     **************************************************************************/


    //ensure address buffer size has enough space for the addresses
    static_assert(library::get_address_buffer_size() >= max(sizeof(in_addr), sizeof(in6_addr)));
   
    
    //invalid network address
    network_address::network_address() {
    }


    //set network address either from ip string or hostname
    network_address::network_address(const char* addr_string, int address_family) {
        
        //requires winsock to be initialized
        get_library();

        //try ip address
        if (inet_pton(address_family, addr_string, m_data) == 1) {
            m_address_family = address_family;
            return;
        }

        addrinfo *ad;

        //resolve address
        int error = getaddrinfo(addr_string, nullptr, nullptr, &ad);

        //error resolving an address
        if (error) {
            throw socket_error(get_last_error(error));
        }

        //find address to return
        for (; ad; ad = ad->ai_next) {
            if (ad->ai_family == address_family) {
                if (address_family == AF_INET) {
                    m_address_family = AF_INET;
                    *reinterpret_cast<in_addr*>(m_data) = reinterpret_cast<const SOCKADDR_IN*>(ad->ai_addr)->sin_addr;
                    return;
                }

                if (address_family == AF_INET6) {
                    m_address_family = AF_INET6;
                    const SOCKADDR_IN6* sa = reinterpret_cast<const SOCKADDR_IN6*>(ad->ai_addr);
                    *reinterpret_cast<in6_addr*>(m_data) = sa->sin6_addr;
                    m_ip6_flow_info = sa->sin6_flowinfo;
                    m_ip6_zone_id = sa->sin6_scope_id;
                    return;
                }

                throw socket_error("unsupported network_address family");
            }
        }

        throw socket_error("invalid network_address");
    }


    //constructor from std::string.
    network_address::network_address(const std::string& addr_string, int address_family)
        : network_address(addr_string.c_str(), address_family) {}


    //converts the address to a string.
    std::string network_address::to_string() const {
        char buffer[64];
        const char* result = nullptr;

        if (m_address_family == AF_INET) {
            result = inet_ntop(AF_INET, reinterpret_cast<const in_addr*>(m_data), buffer, sizeof(buffer));
        }

        else if (m_address_family == AF_INET6) {
            result = inet_ntop(AF_INET6, reinterpret_cast<const in6_addr*>(m_data), buffer, sizeof(buffer));
        }

        else {
            throw socket_error("Unsupported address family");
        }

        if (result) {
            return result;
        }

        throw socket_error(get_last_error());
    }


    /**************************************************************************
        class socket_address
     **************************************************************************/


    //ensure the socket address has enough space for all socket address types.
    static_assert(sizeof(socket_address) >= sizeof(SOCKADDR_STORAGE));


    //invalid socket address constructor.
    socket_address::socket_address() {        
    }


    //constructor from network address and port.
    socket_address::socket_address(const network_address& addr, int port) {

        if (addr.m_address_family == AF_INET) {
            SOCKADDR_IN* sa = reinterpret_cast<SOCKADDR_IN*>(m_data);
            sa->sin_addr = *reinterpret_cast<const IN_ADDR*>(addr.m_data);
            sa->sin_family = AF_INET;
            sa->sin_port = htons(port);
        }

        else if (addr.m_address_family == AF_INET6) {
            SOCKADDR_IN6* sa = reinterpret_cast<SOCKADDR_IN6*>(m_data);
            sa->sin6_addr = *reinterpret_cast<const IN6_ADDR*>(addr.m_data);
            sa->sin6_family = AF_INET6;
            sa->sin6_flowinfo = addr.m_ip6_flow_info;
            sa->sin6_port = htons(port);
            sa->sin6_scope_id = addr.m_ip6_zone_id;
        }
        
        else {
            throw socket_error("Unsupported address family");
        }
    }


    //returns the network address part.
    network_address socket_address::get_address() const {
        const SOCKADDR* sockaddr_base = reinterpret_cast<const SOCKADDR*>(m_data);

        network_address addr;

        if (sockaddr_base->sa_family == AF_INET) {
            const SOCKADDR_IN* sa = reinterpret_cast<const SOCKADDR_IN*>(sockaddr_base);
            addr.m_address_family = AF_INET;
            *reinterpret_cast<in_addr*>(addr.m_data) = sa->sin_addr;
        }

        else if (sockaddr_base->sa_family == AF_INET6) {
            const SOCKADDR_IN6* sa = reinterpret_cast<const SOCKADDR_IN6*>(sockaddr_base);
            addr.m_address_family = AF_INET6;
            *reinterpret_cast<in6_addr*>(addr.m_data) = sa->sin6_addr;
            addr.m_ip6_flow_info = sa->sin6_flowinfo;
            addr.m_ip6_zone_id = sa->sin6_scope_id;
        }

        else {
            throw socket_error("Unsupported address family");
        }

        return addr;
    }


    //returns the port.
    int socket_address::get_port() const {
        const SOCKADDR* sockaddr_base = reinterpret_cast<const SOCKADDR*>(m_data);

        if (sockaddr_base->sa_family == AF_INET) {
            return reinterpret_cast<const SOCKADDR_IN*>(sockaddr_base)->sin_port;
        }

        if (sockaddr_base->sa_family == AF_INET6) {
            return reinterpret_cast<const SOCKADDR_IN6*>(sockaddr_base)->sin6_port;
        }

        throw socket_error("Unsupported address family");
    }


    /**************************************************************************
        class socket 
     **************************************************************************/


    //socket constants
    const int socket::SOCKET_STREAM = SOCK_STREAM;
    const int socket::SOCKET_DATAGRAM = SOCK_DGRAM;
    const int socket::IP_PROTOCOL_TCP = IPPROTO_TCP;
    const int socket::IP_PROTOCOL_UDP = IPPROTO_UDP;
    const int socket::LEVEL_SOCKET = SOL_SOCKET;
    const int socket::LEVEL_TCP = IPPROTO_TCP;
    const int socket::OPTION_REUSE_ADDRESS = SO_REUSEADDR;
    const int socket::OPTION_DISABLE_TCP_SEND_COALESCING = TCP_NODELAY;


    //invalid socket constructor.
    socket::socket() : m_handle(INVALID_SOCKET) {
    }


    //opens a socket.
    socket::socket(int af, int type, int protocol) 
        : socket()
    {
        open(af, type, protocol);
    }


    //opens a socket.
    socket::socket(TYPE type) 
        : socket()
    {
        open(type);
    }


    //moves a socket handle.
    socket::socket(socket&& src)
        : m_handle(src.m_handle)
    {
        src.m_handle = INVALID_SOCKET;
    }


    //shuts down and closes a socket.
    socket::~socket() {
        ::shutdown(m_handle, SD_BOTH);
        ::closesocket(m_handle);
    }


    //moves the handle to this socket.
    socket& socket::operator = (socket&& src) {
        const uintptr_t temp = src.m_handle;
        src.m_handle = INVALID_SOCKET;
        m_handle = temp;
        return *this;
    }


    //checks if the socket is valid.
    socket::operator bool() const {
        return m_handle != INVALID_SOCKET;
    }


    //opens a socket.
    void socket::open(int af, int type, int protocol) {
        if (m_handle != INVALID_SOCKET) {
            throw socket_error("Socket already open");
        }

        m_handle = get_library().create_socket(af, type, protocol);

        m_streaming_socket = type == SOCK_STREAM;
    }


    //opens a socket from type.
    void socket::open(TYPE type) {
        switch (type) {
            case TYPE::TCP_IP4:
                open(ADDRESS_FAMILY_IP4, SOCKET_STREAM, IP_PROTOCOL_TCP);
                break;

            case TYPE::TCP_IP6:
                open(ADDRESS_FAMILY_IP6, SOCKET_STREAM, IP_PROTOCOL_TCP);
                break;

            case TYPE::UDP_IP4:
                open(ADDRESS_FAMILY_IP4, SOCKET_DATAGRAM, IP_PROTOCOL_UDP);
                break;

            case TYPE::UDP_IP6:
                open(ADDRESS_FAMILY_IP6, SOCKET_DATAGRAM, IP_PROTOCOL_UDP);
                break;
        }
    }


    //shuts down the socket.
    void socket::shutdown(bool shutdown_send, bool shutdown_receive) {
        if (shutdown_send && shutdown_receive) {
            ::shutdown(m_handle, SD_BOTH);
        }
        else if (shutdown_send) {
            ::shutdown(m_handle, SD_SEND);
        }
        else if (shutdown_receive) {
            ::shutdown(m_handle, SD_RECEIVE);
        }
        else {
            throw socket_error("Neither send nor receive was shutdown");
        }
    }


    //closes the socket.
    void socket::close() {
        ::closesocket(m_handle);
    }


    //returns an option.
    void socket::get_option(int level, int option_id, byte_buffer& option_value) const {
        int option_value_size = static_cast<int>(option_value.size());
        if (::getsockopt(m_handle, level, option_id, reinterpret_cast<char*>(option_value.data()), &option_value_size) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
        else {
            option_value.resize(option_value_size);
        }
    }


    //sets an option.
    void socket::set_option(int level, int option_id, const byte_buffer& option_value) {
        if (::setsockopt(m_handle, level, option_id, reinterpret_cast<const char*>(option_value.data()), static_cast<int>(option_value.size())) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
    }


    //get the specific option
    bool socket::get_reuse() const {
        return get_option<bool>(LEVEL_SOCKET, OPTION_REUSE_ADDRESS);
    }


    //set the specific option
    void socket::set_reuse(bool v) {
        set_option(LEVEL_SOCKET, OPTION_REUSE_ADDRESS, v);
    }


    //get the specific option
    bool socket::get_disable_tcp_send_coalescing() const {
        return get_option<bool>(LEVEL_TCP, OPTION_DISABLE_TCP_SEND_COALESCING);
    }


    //set the specific option
    void socket::set_disable_tcp_send_coalescing(bool v) {
        set_option<bool>(LEVEL_TCP, OPTION_DISABLE_TCP_SEND_COALESCING, v);
    }


    //sets the socket to listen mode.
    void socket::listen(int backlog) {
        if (::listen(m_handle, backlog == INT_MAX ? SOMAXCONN : backlog) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
    }


    //accepts a connection.
    std::pair<socket, socket_address> socket::accept() {
        std::pair<socket, socket_address> result{ socket{}, socket_address{} };

        int addr_size = sizeof(result.second.m_data);

        result.first = ::accept(m_handle, reinterpret_cast<SOCKADDR*>(result.second.m_data), &addr_size);
        
        if (result.first == INVALID_SOCKET) {
            throw socket_error(get_last_error());
        }

        return result;
    }


    //connects to a remote socket.
    void socket::connect(const socket_address& addr) {
        if (::connect(m_handle, reinterpret_cast<const SOCKADDR*>(addr.m_data), sizeof(addr.m_data)) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
    }


    //binds a socket.
    void socket::bind(const socket_address& addr) {
        if (::bind(m_handle, reinterpret_cast<const SOCKADDR*>(addr.m_data), sizeof(addr.m_data)) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
    }


    //sends data from raw buffer.
    size_t socket::send(const void* buffer, size_t size, int flags) {
        const int result = ::send(m_handle, reinterpret_cast<const char*>(buffer), static_cast<int>(size), flags);

        if (result == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }

        return static_cast<size_t>(result);
    }


    //sends data.
    size_t socket::send(const byte_buffer& buffer, int flags) {
        return send(buffer.data(), buffer.size(), flags);
    }


    //sends data to specific address.
    size_t socket::send(const byte_buffer& buffer, const socket_address& addr, int flags) {
        const int result = ::sendto(m_handle, reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()), flags, reinterpret_cast<const SOCKADDR*>(addr.m_data), sizeof(SOCKADDR_IN)/*sizeof(addr.m_data)*/);
        
        if (result == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
        
        return static_cast<size_t>(result);
    }


    //receives data in raw buffer.
    size_t socket::receive(void* buffer, size_t size, int flags) {
        const int result = ::recv(m_handle, reinterpret_cast<char*>(buffer), static_cast<int>(size), flags);

        if (result == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }

        return static_cast<size_t>(result);
    }


    //receives data.
    size_t socket::receive(byte_buffer& buffer, int flags) {
        return receive(buffer.data(), buffer.size(), flags);
    }


    //receives data and the source address.
    size_t socket::receive(byte_buffer& buffer, socket_address& addr, int flags) {
        int addr_size = sizeof(addr.m_data);

        const int result = ::recvfrom(m_handle, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags, reinterpret_cast<SOCKADDR*>(addr.m_data), &addr_size);
        
        if (result == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
        
        return static_cast<size_t>(result);
    }


} //namespace netlib
