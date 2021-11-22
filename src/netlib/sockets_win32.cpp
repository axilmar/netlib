#include "netlib/socket_set.hpp"
#define FD_SETSIZE netlib::socket_set::MAX_SOCKETS
#include <WinSock2.h>
#include <In6addr.h>
#include <Ws2tcpip.h>
#include <type_traits>
#include "netlib/socket.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/socket_selector.hpp"


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

        //return size of ip_address buffer
        static constexpr size_t get_address_buffer_size() {
            return ip_address::BUFFER_SIZE;
        }

    private:
        //init library
        library() {
            WSADATA wsaData;
            const int error = WSAStartup(MAKEWORD(2, 0), &wsaData);
            if (error != NO_ERROR) {
                throw socket_error(stringstream() << "Window Sockets library could not be initialized; error = " << error);
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
        return stringstream() << '[' << error_id << "] " << msg_str;
    }


    /**************************************************************************
        class constants
     **************************************************************************/


    //constants
    const int constants::ADDRESS_FAMILY_IP4 = AF_INET;
    const int constants::ADDRESS_FAMILY_IP6 = AF_INET6;


    /**************************************************************************
        class ip_address
     **************************************************************************/


    //ensure address buffer size has enough space for the addresses
    static_assert(library::get_address_buffer_size() == max(sizeof(in_addr), sizeof(in6_addr)));
   
    
    //Returns the host name of this machine.
    std::string ip_address::get_host_name() {
        //temp buffer
        char buffer[256];

        //get the name
        if (gethostname(buffer, sizeof(buffer)) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }

        //find the string length
        size_t string_length = strlen(buffer);

        //return the string
        return std::string(buffer, buffer + string_length);
    }


    //Creates a network address of the given address family.
    ip_address::ip_address(int af) 
        : m_address_family(af)
    {
    }


    //set network address either from ip string or hostname
    ip_address::ip_address(const char* addr_string, int address_family) 
        : ip_address(std::string(addr_string), address_family)
    {
    }


    //set network address either from ip string or hostname
    ip_address::ip_address(std::string addr_string, int address_family) {        
        //requires winsock to be initialized
        get_library();

        //if the string is empty, get the host name
        if (addr_string.empty()) {
            addr_string = get_host_name();
        }

        //try ip address
        if (inet_pton(address_family, addr_string.c_str(), m_data) == 1) {
            m_address_family = address_family;
            return;
        }

        addrinfo *ad;

        //resolve address
        int error = getaddrinfo(addr_string.c_str(), nullptr, nullptr, &ad);

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

                throw socket_error("unsupported address family");
            }
        }

        throw socket_error("invalid address");
    }


    //Creates a network address for any address.
    ip_address::ip_address(any_address_type address, int address_family) {
        if (address_family == constants::ADDRESS_FAMILY_IP4) {
            m_address_family = constants::ADDRESS_FAMILY_IP4;
            *reinterpret_cast<decltype(INADDR_ANY)*>(m_data) = INADDR_ANY;
        }
        else if (address_family == constants::ADDRESS_FAMILY_IP6) {
            m_address_family = constants::ADDRESS_FAMILY_IP6;
            *reinterpret_cast<std::decay_t<decltype(in6addr_any)>*>(m_data) = in6addr_any;
        }
        else {
            throw socket_error("unsupported address family");
        }
    }


    //Creates a network address for the loopback address.
    ip_address::ip_address(loopback_address_type address, int address_family) {
        if (address_family == constants::ADDRESS_FAMILY_IP4) {
            m_address_family = constants::ADDRESS_FAMILY_IP4;
            *reinterpret_cast<decltype(INADDR_ANY)*>(m_data) = INADDR_LOOPBACK;
        }
        else if (address_family == constants::ADDRESS_FAMILY_IP6) {
            m_address_family = constants::ADDRESS_FAMILY_IP6;
            *reinterpret_cast<std::decay_t<decltype(in6addr_loopback)>*>(m_data) = in6addr_loopback;
        }
        else {
            throw socket_error("unsupported address family");
        }
    }


    //converts the address to a string.
    std::string ip_address::to_string() const {
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
    static_assert(socket_address::BUFFER_SIZE >= sizeof(SOCKADDR_STORAGE));


    //constructor from network address and port.
    socket_address::socket_address(const ip_address& addr, int port) {

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
    ip_address socket_address::get_address() const {
        const SOCKADDR* sockaddr_base = reinterpret_cast<const SOCKADDR*>(m_data);

        ip_address addr;

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
    const int socket::OPTION_TYPE = SO_TYPE;


    //opens a socket.
    void socket::open(int af, int type, int protocol) {
        if (m_handle != INVALID_SOCKET) {
            throw socket_error("Socket already open");
        }

        m_handle = get_library().create_socket(af, type, protocol);

        if (m_handle == INVALID_SOCKET) {
            throw socket_error(get_last_error());
        }
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
        m_handle = INVALID_SOCKET;
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

        int addr_size = static_cast<int>(result.second.size());

        result.first = ::accept(m_handle, reinterpret_cast<SOCKADDR*>(result.second.data()), &addr_size);
        
        if (result.first) {
            return result;
        }

        throw socket_error(get_last_error());
    }


    //connects to a remote socket.
    void socket::connect(const socket_address& addr) {
        if (::connect(m_handle, reinterpret_cast<const SOCKADDR*>(addr.data()), static_cast<int>(addr.size())) == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }
    }


    //binds a socket.
    void socket::bind(const socket_address& addr) {
        if (::bind(m_handle, reinterpret_cast<const SOCKADDR*>(addr.data()), static_cast<int>(addr.size())) == SOCKET_ERROR) {
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


    //sends data from raw buffer to specific address.
    size_t socket::send(const void* buffer, size_t size, const socket_address& addr, int flags) {
        const int result = ::sendto(m_handle, reinterpret_cast<const char*>(buffer), static_cast<int>(size), flags, reinterpret_cast<const SOCKADDR*>(addr.data()), static_cast<int>(addr.size()));

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


    //receives data in raw buffer from specific address.
    size_t socket::receive(void* buffer, size_t size, socket_address& addr, int flags) {
        int addr_len = static_cast<int>(addr.size());
        const int result = ::recvfrom(m_handle, reinterpret_cast<char*>(buffer), static_cast<int>(size), flags, reinterpret_cast<SOCKADDR*>(addr.data()), &addr_len);

        if (result == SOCKET_ERROR) {
            throw socket_error(get_last_error());
        }

        return static_cast<size_t>(result);
    }


    /**************************************************************************
        class socket_set
     **************************************************************************/


    //make sure the socket set buffer has enough room.
    static_assert(socket_set::BUFFER_SIZE >= sizeof(fd_set));


    //Initializes the socket set.
    socket_set::socket_set() {
        FD_ZERO(m_data);
    }


    //Checks if it includes the socket.
    bool socket_set::contains(socket& s) const {
        return m_socket_map.find(s.m_handle) != m_socket_map.end();
    }


    //adds a socket.
    void socket_set::add(socket& s) {
        if (contains(s)) {
            throw socket_error("socket already in set.");
        }
        FD_SET(s.m_handle, m_data);
        m_socket_map[s.m_handle] = &s;
    }


    //removes a socket.
    void socket_set::remove(socket& s) {
        if (!contains(s)) {
            throw socket_error("socket not in set.");
        }
        FD_CLR(s.m_handle, m_data);
        m_socket_map.erase(s.m_handle);
    }


    //clears the socket set.
    void socket_set::clear() {
        FD_ZERO(m_data);
        m_socket_map.clear();
    }


    /**************************************************************************
        class socket_selector
     **************************************************************************/


    //internal fd set and copy initialization
    static fd_set* copy_construct_fd_set(fd_set* dst, const fd_set* src) {
        //if the set contains no sockets, return null
        if (src->fd_count == 0) {
            return nullptr;
        }

        //init destination set
        FD_ZERO(dst);

        //copy entries
        memcpy(dst->fd_array, src->fd_array, src->fd_count * sizeof(SOCKET));

        //copy count
        dst->fd_count = src->fd_count;

        //return destination
        return dst;
    }


    //sets the result set
    static void set_result_socket_set(const std::map<uintptr_t, socket*>& socket_map, fd_set* socket_descriptor_set, std::vector<socket*>& sockets) {
        sockets.clear();

        //if the socket descriptor set did not contain any sockets, return empty set
        if (!socket_descriptor_set || socket_descriptor_set->fd_count == 0) {
            return;
        }

        //go through the socket array and return set sockets
        for (size_t i = 0; i < socket_descriptor_set->fd_count; ++i) {
            const uintptr_t socket_handle = socket_descriptor_set->fd_array[i];
            if (socket_handle != INVALID_SOCKET) {
                auto it = socket_map.find(socket_handle);
                if (it != socket_map.end()) {
                    sockets.push_back(it->second);
                }
            }
        }
    }


    //conversion of microseconds to TIMEVAL
    static TIMEVAL microseconds_to_timeval(const std::chrono::microseconds& us) {
        return { static_cast<long>(us.count() / 1'000'000ll), static_cast<long>(us.count() % 1'000'000ll) };
    }


    //internal select function
    template <class F>
    static bool _select(std::shared_mutex& mutex, const socket_set& read_socket_set, const socket_set& write_socket_set, const socket_set& except_socket_set, std::vector<socket*>& read_sockets, std::vector<socket*>& write_sockets, std::vector<socket*>& except_sockets, F&& func) {
        fd_set read_set, write_set, except_set;
        fd_set* read_set_ptr, * write_set_ptr, * except_set_ptr;

        //get sets
        {
            std::shared_lock read_lock(mutex);
            read_set_ptr = copy_construct_fd_set(&read_set, reinterpret_cast<const fd_set*>(read_socket_set.data()));
            write_set_ptr = copy_construct_fd_set(&write_set, reinterpret_cast<const fd_set*>(write_socket_set.data()));
            except_set_ptr = copy_construct_fd_set(&except_set, reinterpret_cast<const fd_set*>(except_socket_set.data()));
        }

        //wait
        if (!func(read_set_ptr, write_set_ptr, except_set_ptr)) {
            return false;
        }

        //set result sets
        {
            std::shared_lock read_lock(mutex);
            set_result_socket_set(read_socket_set.get_socket_map(), read_set_ptr, read_sockets);
            set_result_socket_set(write_socket_set.get_socket_map(), write_set_ptr, write_sockets);
            set_result_socket_set(except_socket_set.get_socket_map(), except_set_ptr, except_sockets);
        }

        //success
        return true;
    }


    //waits for sockets
    void socket_selector::wait(std::vector<socket*>& read_sockets, std::vector<socket*>& write_sockets, std::vector<socket*>& except_sockets) {
        _select(m_mutex, m_read_set, m_write_set, m_except_set, read_sockets, write_sockets, except_sockets, [](fd_set* read_set, fd_set* write_set, fd_set* except_set) {

            //invoke select
            const int result = select(0, read_set, write_set, except_set, nullptr);

            //error
            if (result == SOCKET_ERROR) {
                throw socket_error(get_last_error());
            }

            //no error
            return true;
        });
    }


    //Waits for any socket to be ready, with a timeout.
    bool socket_selector::wait(std::vector<socket*>& read_sockets, std::vector<socket*>& write_sockets, std::vector<socket*>& except_sockets, const std::chrono::microseconds& timeout) {
        return _select(m_mutex, m_read_set, m_write_set, m_except_set, read_sockets, write_sockets, except_sockets, [&](fd_set* read_set, fd_set* write_set, fd_set* except_set) {

            //convert microseconds to timeval
            const TIMEVAL tv = microseconds_to_timeval(timeout);

            //invoke select
            const int result = select(0, read_set, write_set, except_set, &tv);

            //timeout
            if (result == 0) {
                return false;
            }

            //error
            if (result == SOCKET_ERROR) {
                throw socket_error(get_last_error());
            }

            //no error
            return true;
        });
    }


} //namespace netlib
