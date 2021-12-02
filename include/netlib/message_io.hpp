#ifndef NETLIB_MESSAGE_IO_HPP
#define NETLIB_MESSAGE_IO_HPP


#include <limits>
#include "socket.hpp"
#include "message.hpp"
#include "message_size.hpp"
#include "message_registry.hpp"
#include "pipe.hpp"


/**
 * max message size preprocessor definition. 
 */
#ifndef NETLIB_MAX_MESSAGE_SIZE
#define NETLIB_MAX_MESSAGE_SIZE 4096
#endif


namespace netlib {


    ///////////////////////////////////////////////////////////////////////////
    // common functions
    ///////////////////////////////////////////////////////////////////////////


    /**
     * Returns a thread-local intermediate buffer.
     */
    std::vector<char>& intermediate_buffer() { 
        static thread_local std::vector<char> buffer;
        return buffer;
    }


    ///////////////////////////////////////////////////////////////////////////
    // socket functions
    ///////////////////////////////////////////////////////////////////////////


    /**
     * Sends data over a tcp socket.
     * It sends all the data.
     * If there is not enough space in the system buffer, then it blocks until there is.
     * @param socket socket to use for sending data.
     * @param data data to send.
     * @param size number of bytes to send.
     * @return true if the data were sent, false if the connection was closed.
     */
    inline bool tcp_send(socket& s, const void* data, size_t size) {
        while (size) {
            const size_t bytes_sent = s.send(data, size);
            
            if (bytes_sent == socket::nsize) {
                return false;
            }

            size -= bytes_sent;
            reinterpret_cast<const char*&>(data) += bytes_sent;
        }

        return true;
    }


    /**
     * Receives data over a tcp socket.
     * If the system buffer is empty, then it blocks until it has some data.
     * @param s socket to receive data from.
     * @param data destination buffer.
     * @param size number of bytes to receive.
     * @return true if the data were received, false if the connection was closed.
     */
    inline bool tcp_receive(socket& s, void* data, size_t size) {
        while (size) {
            const size_t bytes_received = s.receive(data, size);

            if (bytes_received == socket::nsize) {
                return false;
            }

            size -= bytes_received;
            reinterpret_cast<char*&>(data) += bytes_received;
        }

        return true;
    }


    /**
     * Sends a message over a tcp socket.
     * It sends the message size, before the message, and waits for all the data to be send, before returning.
     * @param s socket to send the message over.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class Encrypt, class... T> bool tcp_send_message(socket& s, const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer = intermediate_buffer()) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //check the size
        if (buffer.size() > std::numeric_limits<message_size>::max()) {
            throw std::out_of_range("Buffer size too big for message size type.");
        }

        //send the size
        message_size size = static_cast<message_size>(buffer.size());
        encrypt(&size, sizeof(size));
        if (!tcp_send(s, &size, sizeof(size))) {
            return false;
        }

        //send the data
        encrypt(buffer.data(), buffer.size());
        return tcp_send(s, buffer.data(), buffer.size());
    }


    /**
     * Sends a message without encryption.
     * @param s socket to send the message over.
     * @param msg message to send.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class... T> bool tcp_send_message(socket& s, const message<T...>& msg, std::vector<char>& buffer = intermediate_buffer()) {
        return tcp_send_message(s, msg, [](void*, size_t) {}, buffer);
    }


    /**
     * Receives a message over a tcp socket.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    template <class Decrypt> message_ptr tcp_receive_message(socket& s, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        //receive the size
        message_size size;
        if (!tcp_receive(s, &size, sizeof(size))) {
            return nullptr;
        }
        decrypt(&size, sizeof(size));

        //receive the data
        buffer.resize(size);
        if (!tcp_receive(s, buffer.data(), buffer.size())) {
            return nullptr;
        }
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Receives a message over a tcp socket without decryption.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    inline message_ptr tcp_receive_message(socket& s, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        return tcp_receive_message(s, [](void*, size_t) {}, buffer, memory_resource);
    }


    /**
     * Sends a message over a udp socket.
     * It sends the message size, before the message, and waits for all the data to be send, before returning.
     * @param s socket to send the message over.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class Encrypt, class... T> bool udp_send_message(socket& s, const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer = intermediate_buffer()) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //encrypt the data
        encrypt(buffer.data(), buffer.size());

        //send the data
        return s.send(buffer.data(), buffer.size()) == buffer.size();
    }


    /**
     * Sends a message without encryption.
     * @param s socket to send the message over.
     * @param msg message to send.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class... T> bool udp_send_message(socket& s, const message<T...>& msg, std::vector<char>& buffer = intermediate_buffer()) {
        return udp_send_message(s, msg, [](void*, size_t) {}, buffer);
    }


    /**
     * Receives a message over a udp socket.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param max_message_size maximum message size.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    template <class Decrypt> message_ptr udp_receive_message(socket& s, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = NETLIB_MAX_MESSAGE_SIZE) {
        //receive the data
        buffer.resize(max_message_size);
        const size_t received_bytes = s.receive(buffer.data(), buffer.size());

        //if no bytes where received, return null
        if (received_bytes == socket::nsize) {
            return nullptr;
        }

        //make sure the buffer contains the appropriate number of bytes
        buffer.resize(received_bytes);

        //decrypt the data
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Receives a message over a udp socket without decryption.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param max_message_size maximum message size.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    inline message_ptr udp_receive_message(socket& s, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = NETLIB_MAX_MESSAGE_SIZE) {
        return udp_receive_message(s, [](void*, size_t) {}, buffer, memory_resource, max_message_size);
    }


    /**
     * Sends a message over a udp socket.
     * It sends the message size, before the message, and waits for all the data to be send, before returning.
     * @param s socket to send the message over.
     * @param addr address of receiver.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class Encrypt, class... T> bool udp_send_message(socket& s, const socket_address& addr, const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer = intermediate_buffer()) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //encrypt the data
        encrypt(buffer.data(), buffer.size());

        //send the data
        return s.send(buffer.data(), buffer.size(), addr) == buffer.size();
    }


    /**
     * Sends a message without encryption.
     * @param s socket to send the message over.
     * @param addr address of receiver.
     * @param msg message to send.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the socket was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class... T> bool udp_send_message(socket& s, const socket_address& addr, const message<T...>& msg, std::vector<char>& buffer = intermediate_buffer()) {
        return udp_send_message(s, addr, msg, [](void*, size_t) {}, buffer);
    }


    /**
     * Receives a message over a udp socket.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param addr address of sender.
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param max_message_size maximum message size.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    template <class Decrypt> message_ptr udp_receive_message(socket& s, socket_address& addr, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = NETLIB_MAX_MESSAGE_SIZE) {
        //receive the data
        buffer.resize(max_message_size);
        const size_t received_bytes = s.receive(buffer.data(), buffer.size(), addr);

        //if no bytes where received, return null
        if (received_bytes == socket::nsize) {
            return nullptr;
        }

        //make sure the buffer contains the appropriate number of bytes
        buffer.resize(received_bytes);

        //decrypt the data
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Receives a message over a udp socket without decryption.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param s socket to use.
     * @param addr address of sender.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param max_message_size maximum message size.
     * @return pointer to message or a null ptr if the socket is closed.
     */
    inline message_ptr udp_receive_message(socket& s, socket_address& addr, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = NETLIB_MAX_MESSAGE_SIZE) {
        return udp_receive_message(s, addr, [](void*, size_t) {}, buffer, memory_resource, max_message_size);
    }


    ///////////////////////////////////////////////////////////////////////////
    // pipe functions
    ///////////////////////////////////////////////////////////////////////////


    /**
     * Sends data over a pipe.
     * It sends all the data.
     * If there is not enough space in the system buffer, then it blocks until there is.
     * @param pipe pipe to use for sending data.
     * @param data data to send.
     * @param size number of bytes to send.
     * @return true if the data were sent, false if the connection was closed.
     */
    inline bool pipe_send(pipe& p, const void* data, size_t size) {
        while (size) {
            const auto [bytes_sent, open] = p.write(data, size);

            if (!open) {
                return false;
            }

            size -= bytes_sent;
            reinterpret_cast<const char*&>(data) += bytes_sent;
        }

        return true;
    }


    /**
     * Receives data over a pipe.
     * If the system buffer is empty, then it blocks until it has some data.
     * @param p pipe to receive data from.
     * @param data destination buffer.
     * @param size number of bytes to receive.
     * @return true if the data were received, false if the connection was closed.
     */
    inline bool pipe_receive(pipe& p, void* data, size_t size) {
        while (size) {
            const auto [bytes_received, open] = p.read(data, size);

            if (!open) {
                return false;
            }

            size -= bytes_received;
            reinterpret_cast<char*&>(data) += bytes_received;
        }

        return true;
    }


    /**
     * Sends a message over a pipe.
     * It sends the message size, before the message, and waits for all the data to be send, before returning.
     * @param p pipe to send the message over.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the pipe was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class Encrypt, class... T> bool pipe_send_message(pipe& p, const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer = intermediate_buffer()) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //check the size
        if (buffer.size() > std::numeric_limits<message_size>::max()) {
            throw std::out_of_range("Buffer size too big for message size type.");
        }

        //send the size
        message_size size = static_cast<message_size>(buffer.size());
        encrypt(&size, sizeof(size));
        if (!pipe_send(p, &size, sizeof(size))) {
            return false;
        }

        //send the data
        encrypt(buffer.data(), buffer.size());
        return pipe_send(p, buffer.data(), buffer.size());
    }


    /**
     * Sends a message without encryption.
     * @param p pipe to send the message over.
     * @param msg message to send.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the pipe was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class... T> bool pipe_send_message(pipe& p, const message<T...>& msg, std::vector<char>& buffer = intermediate_buffer()) {
        return pipe_send_message(p, msg, [](void*, size_t) {}, buffer);
    }


    /**
     * Receives a message over a pipe.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param p pipe to use.
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the pipe is closed.
     */
    template <class Decrypt> message_ptr pipe_receive_message(pipe& p, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        //receive the size
        message_size size;
        if (!pipe_receive(p, &size, sizeof(size))) {
            return nullptr;
        }
        decrypt(&size, sizeof(size));

        //receive the data
        buffer.resize(size);
        if (!pipe_receive(p, buffer.data(), buffer.size())) {
            return nullptr;
        }
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Receives a message over a pipe without decryption.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param p pipe to use.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the pipe is closed.
     */
    inline message_ptr pipe_receive_message(pipe& p, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        return pipe_receive_message(p, [](void*, size_t) {}, buffer, memory_resource);
    }


    ///////////////////////////////////////////////////////////////////////////
    // io_resource functions
    ///////////////////////////////////////////////////////////////////////////


    /**
     * Sends data over an io_resource.
     * @param io_resource io_resource to use for sending data.
     * @param data data to send.
     * @param size number of bytes to send.
     * @return true if the data were sent, false if the connection was closed.
     */
    inline bool io_resource_write(io_resource& r, const void* data, size_t size) {
        while (size) {
            const auto [bytes_sent, open] = r.write(data, size);

            if (!open) {
                return false;
            }

            size -= bytes_sent;
            reinterpret_cast<const char*&>(data) += bytes_sent;
        }

        return true;
    }


    /**
     * Receives data over a io_resource.
     * @param r io_resource to receive data from.
     * @param data destination buffer.
     * @param size number of bytes to receive.
     * @return true if the data were received, false if the connection was closed.
     */
    inline bool io_resource_read(io_resource& r, void* data, size_t size) {
        while (size) {
            const auto [bytes_received, open] = r.read(data, size);

            if (!open) {
                return false;
            }

            size -= bytes_received;
            reinterpret_cast<char*&>(data) += bytes_received;
        }

        return true;
    }


    /**
     * Sends a message over a io_resource.
     * It sends the message size, before the message, and waits for all the data to be send, before returning.
     * @param r io_resource to send the message over.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the io_resource was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class Encrypt, class... T> bool io_resource_write_message(io_resource& r, const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer = intermediate_buffer()) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //check the size
        if (buffer.size() > std::numeric_limits<message_size>::max()) {
            throw std::out_of_range("Buffer size too big for message size type.");
        }

        //send the size
        message_size size = static_cast<message_size>(buffer.size());
        encrypt(&size, sizeof(size));
        if (!io_resource_write(r, &size, sizeof(size))) {
            return false;
        }

        //send the data
        encrypt(buffer.data(), buffer.size());
        return io_resource_write(r, buffer.data(), buffer.size());
    }


    /**
     * Sends a message without encryption.
     * @param r io_resource to send the message over.
     * @param msg message to send.
     * @param buffer intermediate buffer used for serialization.
     * @return true if the data were sent, false if the io_resource was closed.
     * @exception std::out_of_range thrown if the message is too big for the current message size.
     */
    template <class... T> bool io_resource_write_message(io_resource& r, const message<T...>& msg, std::vector<char>& buffer = intermediate_buffer()) {
        return io_resource_write_message(r, msg, [](void*, size_t) {}, buffer);
    }


    /**
     * Receives a message over a io_resource.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param r io_resource to use.
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the io_resource is closed.
     */
    template <class Decrypt> message_ptr io_resource_read_message(io_resource& r, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        //receive the size
        message_size size;
        if (!io_resource_read(r, &size, sizeof(size))) {
            return nullptr;
        }
        decrypt(&size, sizeof(size));

        //receive the data
        buffer.resize(size);
        if (!io_resource_read(r, buffer.data(), buffer.size())) {
            return nullptr;
        }
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Receives a message over a io_resource without decryption.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param r io_resource to use.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @return pointer to message or a null ptr if the io_resource is closed.
     */
    inline message_ptr io_resource_read_message(io_resource& r, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource()) {
        return io_resource_read_message(r, [](void*, size_t) {}, buffer, memory_resource);
    }


} //namespace netlib


#endif //NETLIB_MESSAGE_IO_HPP
