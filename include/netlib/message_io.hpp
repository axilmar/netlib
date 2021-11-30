#ifndef NETLIB_MESSAGE_IO_HPP
#define NETLIB_MESSAGE_IO_HPP


#include <limits>
#include "socket.hpp"
#include "message.hpp"
#include "message_size.hpp"
#include "message_registry.hpp"


namespace netlib {


    /**
     * Returns a thread-local intermediate buffer.
     */
    std::vector<char>& intermediate_buffer() { 
        static thread_local std::vector<char> buffer;
        return buffer;
    }


    /**
     * Sends data over a tcp socket.
     * It sends all the data.
     * If there is not enough space in the system buffer, then it blocks until there is.
     * @param socket socket to use for sending data.
     * @param data data to send.
     * @param size number of bytes to send.
     * @return true if the data were sent, false if the socket was closed.
     */
    inline bool tcp_send(socket& s, const void* data, size_t size) {
        while (size) {
            const size_t bytes_sent = s.send(data, size);
            
            if (!bytes_sent) {
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
     * @return true if the data were received, false if the socket was closed.
     */
    inline bool tcp_receive(socket& s, void* data, size_t size) {
        while (size) {
            const size_t bytes_received = s.receive(data, size);

            if (!bytes_received) {
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

        //send the data
        encrypt(buffer.data(), buffer.size());
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
    template <class Decrypt> message_ptr udp_receive_message(socket& s, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = 4096) {
        //receive the data
        buffer.resize(max_message_size);
        const size_t received_bytes = s.receive(buffer.data(), buffer.size());
        if (!received_bytes) {
            return nullptr;
        }
        buffer.resize(received_bytes);
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
    inline message_ptr udp_receive_message(socket& s, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = 4096) {
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

        //send the data
        encrypt(buffer.data(), buffer.size());
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
    template <class Decrypt> message_ptr udp_receive_message(socket& s, socket_address& addr, Decrypt&& decrypt, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = 4096) {
        //receive the data
        buffer.resize(max_message_size);
        const size_t received_bytes = s.receive(buffer.data(), buffer.size(), addr);
        if (!received_bytes) {
            return nullptr;
        }
        buffer.resize(received_bytes);
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
    inline message_ptr udp_receive_message(socket& s, socket_address& addr, std::vector<char>& buffer = intermediate_buffer(), std::pmr::memory_resource& memory_resource = *std::pmr::get_default_resource(), const size_t max_message_size = 4096) {
        return udp_receive_message(s, addr, [](void*, size_t) {}, buffer, memory_resource, max_message_size);
    }


} //namespace netlib


#endif //NETLIB_MESSAGE_IO_HPP
