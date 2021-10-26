#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <cstdint>
#include <utility>
#include "socket_error.hpp"
#include "socket_address.hpp"
#include "byte_buffer.hpp"


namespace netlib {


    class socket {
    public:
        socket(int af, int type, int protocol);

        socket(const socket&) = delete;

        socket(socket&& src);

        ~socket();

        socket& operator = (const socket&) = delete;

        socket& operator = (socket&& src);

        operator bool() const;

        void listen(int backlog = INT_MAX);
        
        std::pair<socket, socket_address> accept();

        void connect(const socket_address& addr);

        void bind(const socket_address& addr);

        size_t send(const byte_buffer& buffer, int flags = 0);

        size_t receive(byte_buffer& buffer, int flags = 0);

        size_t receive(byte_buffer& buffer, socket_address& addr, int flags = 0);

    private:
        uintptr_t m_handle;

        socket(uintptr_t handle = 0) : m_handle(handle) {}
    };


} //namespace netlib


#endif //NETLIB_SOCKET_HPP
