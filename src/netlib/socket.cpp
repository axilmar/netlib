#include "platform.hpp"
#include <stdexcept>
#include <system_error>
#include "netlib/socket.hpp"


namespace netlib {


    //internal reference-counted socket handle structure
    struct socket::data {
        //ref count
        std::atomic<size_t> ref_count;

        //handle
        handle_type handle;

        //constructor
        data(handle_type h = invalid_handle) 
            : ref_count{ 1 }, handle{ h }
        {
        }

        //destructor
        ~data() {
            if (handle != invalid_handle) {
                closesocket(handle);
            }
        }
    };


    //the default constructor.
    socket::socket(handle_type handle) 
        : m_data(new data(handle))
    {
    }


    //closes the socket.
    socket::~socket() {
        unref();
    }


    //The copy constructor.
    socket::socket(const socket& src) 
        : m_data(src.m_data)
    {
        ref();
    }


    //The move constructor.
    socket::socket(socket&& src) 
        : m_data(src.m_data)
    {
        src.m_data = new data{};
    }


    //The copy assignment operator.
    socket& socket::operator = (const socket& src) {
        if (&src != this) {
            unref();
            m_data = src.m_data;
            ref();
        }
        return *this;
    }


    //The move assignment operator.
    socket& socket::operator = (socket&& src) {
        if (&src != this) {
            m_data = src.m_data;
            src.m_data = new data{};
        }
        return *this;
    }


    //Returns the handle.
    socket::handle_type socket::handle() const { 
        return m_data->handle;
    }


    //if socket is initialized
    socket::operator bool() const {
        return handle() != invalid_handle;
    }


    //Returns the address this socket is bound to.
    socket_address socket::bound_address() const {
        sockaddr_storage s;
        int namelen = sizeof(s);

        if (getsockname(handle(), reinterpret_cast<sockaddr*>(&s), &namelen)) {
            throw std::system_error(get_last_error_number(), std::system_category(), get_last_error_message());
        }

        switch (s.ss_family) {
        case AF_INET:
            return socket_address(ntohl(reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(&s)->sin_addr)), ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));

        case AF_INET6:
            return socket_address({ reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(&s)->sin6_addr), reinterpret_cast<const sockaddr_in6*>(&s)->sin6_scope_id }, ntohs(reinterpret_cast<const sockaddr_in*>(&s)->sin_port));
        }

        throw std::logic_error("Invalid address family.");
    }


    //increments the ref count
    void socket::ref() {
        m_data->ref_count.fetch_add(1, std::memory_order_relaxed);
    }


    //decrements the ref count and deletes the socket/data block if ref count reaches 0
    void socket::unref() {
        if (m_data->ref_count.fetch_sub(1, std::memory_order_acquire) == 1) {
            delete m_data;
        }
    }


} //namespace netlib
