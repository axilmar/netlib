#include "netlib/socket.hpp"


namespace netlib {


    //invalid socket constructor.
    socket::socket() : m_handle(-1), m_streaming_socket(false) {
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
        , m_streaming_socket(src.m_streaming_socket)
    {
        src.m_handle = -1;
    }


    //shuts down and closes a socket.
    socket::~socket() {
        shutdown();
        close();
    }


    //moves the handle to this socket.
    socket& socket::operator = (socket&& src) {
        const uintptr_t temp = src.m_handle;
        src.m_handle = -1;
        m_handle = temp;
        m_streaming_socket = src.m_streaming_socket;
        return *this;
    }


    //checks if the socket is valid.
    socket::operator bool() const {
        return m_handle != -1;
    }


    //sends data.
    size_t socket::send(const byte_buffer& buffer, int flags) {
        return send(buffer.data(), buffer.size(), flags);
    }


    //receives data.
    size_t socket::receive(byte_buffer& buffer, int flags) {
        return receive(buffer.data(), buffer.size(), flags);
    }


    //Does not return until all the data are sent.
    bool socket::stream_send(const void* buffer, size_t size, int flags) {
        const char* data = reinterpret_cast<const char*>(buffer);

        for (;;) {
            //send the data
            const size_t sent_bytes = send(data, size, flags);

            //no data could be sent
            if (sent_bytes == 0) {
                return false;
            }

            //calculate remaining amount of data
            size -= sent_bytes;

            //no remaining amount of data; return true
            if (size == 0) {
                return true;
            }

            //adjust the data pointer by the number of sent bytes
            data += sent_bytes;
        }
    }


    //Does not return until all the data are received.
    bool socket::stream_receive(void* buffer, size_t size, int flags) {
        char* data = reinterpret_cast<char*>(buffer);

        for (;;) {
            //receive the data
            const size_t received_bytes = receive(data, size, flags);

            //no data could be received
            if (received_bytes == 0) {
                return false;
            }

            //calculate remaining amount of data
            size -= received_bytes;

            //no remaining amount of data; return true
            if (size == 0) {
                return true;
            }

            //adjust the data pointer by the number of received bytes
            data += received_bytes;
        }
    }


} //namespace netlib
