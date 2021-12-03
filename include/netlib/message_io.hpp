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


    /**
     * Sends data over a stream.
     * @param data data to send.
     * @param size number of bytes to send.
     * @param nsize the value for 'no-size' that indicates the resource is closed.
     * @param send send function; must return size of sent data or 'nsize' if the resource is closed.
     * @return true if the data were sent, false if the connection was closed.
     */
    template <class F> bool stream_send(const void* data, size_t size, const size_t nsize, F&& send) {
        while (size) {
            //send data
            const size_t bytes_sent = send(data, size);

            //if there was no error, send the rest of the data
            if (bytes_sent != nsize) {
                size -= bytes_sent;
                reinterpret_cast<const char*&>(data) += bytes_sent;
                continue;
            }

            //error
            return false;
        }

        return true;
    }


    /**
     * Receives data over a stream.
     * @param data destination buffer.
     * @param size number of bytes to receive.
     * @param nsize the value for 'no-size' that indicates the resource is closed.
     * @param receive the receive function; returns size of received data or 'nsize' if the resource is closed.
     * @return true if the data were received, false if the connection was closed.
     */
    template <class F> bool stream_receive(void* data, size_t size, const size_t nsize, F&& receive) {
        while (size) {
            //receive bytes
            const size_t bytes_received = receive(data, size);

            //if there was no error, receive the rest of the data
            if (bytes_received != nsize) {
                size -= bytes_received;
                reinterpret_cast<char*&>(data) += bytes_received;
                continue;
            }

            //error
            return false;
        }

        return true;
    }


    /**
     * Serializes the given message's size along with the message,
     * in order to send it over a stream-oriented connection (for example, a tcp socket or a pipe).
     * The size is required so as that the receive routine knows how exactly many bytes to extract
     * from the stream. This is not needed for packet-oriented connections, such as udp,
     * because the packet itself contains the size.
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer.
     * @param send function to use for sending the data.
     * @return what the send function returns.
     */
    template <class Encrypt, class F, class... T> bool stream_send_message(const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer, F&& send) {
        buffer.clear();

        //leave room for the message size
        buffer.resize(sizeof(message_size));

        //serialize the message
        msg.serialize(buffer);

        //check the size
        if (buffer.size() > std::numeric_limits<message_size>::max()) {
            throw std::out_of_range("Buffer size too big for message size type.");
        }

        //set the size in the start of the buffer
        const message_size size = static_cast<message_size>(buffer.size() - sizeof(size));
        swap_endianess(buffer.data(), size);

        //encrypt the size
        encrypt(buffer.data(), sizeof(size));

        //encrypt the data
        encrypt(buffer.data() + sizeof(size), buffer.size() - sizeof(size));

        //send the data
        return send(buffer.data(), buffer.size());
    }


    /**
     * Receives a message over a stream.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param receive the receive function; must return true if data are sent, or false if the resource is closed.
     * @return pointer to message or a null ptr if the io_resource is closed.
     */
    template <class Decrypt, class F> message_ptr stream_receive_message(Decrypt&& decrypt, std::vector<char>& buffer, std::pmr::memory_resource& memory_resource, F&& receive) {
        //receive the size
        message_size size;
        if (!receive(&size, sizeof(size))) {
            return nullptr;
        }

        //decrypt the size
        decrypt(&size, sizeof(size));

        //swap endianess of size
        swap_endianess(size);

        //resize the buffer to the received size
        buffer.resize(size);

        //receive the data in the buffer
        if (!receive(buffer.data(), buffer.size())) {
            return nullptr;
        }

        //decrypt the data
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    /**
     * Serializes the message, in order to send it over a packet-oriented connection (for example, a udp socket).
     * @param msg message to send.
     * @param encrypt the encrypt function.
     * @param buffer intermediate buffer.
     * @param send function to use for sending the data.
     * @return what the send function returns.
     */
    template <class Encrypt, class F, class... T> bool packet_send_message(const message<T...>& msg, Encrypt&& encrypt, std::vector<char>& buffer, F&& send) {
        buffer.clear();

        //serialize the message
        msg.serialize(buffer);

        //encrypt the data
        encrypt(buffer.data(), buffer.size());

        //send the data
        return send(buffer.data(), buffer.size());
    }


    /**
     * Receives a message over a packet-oriented connection.
     * The message to be received must have been registered with the message registry (automatic when using message<T...>).
     * @param decrypt decrypt function.
     * @param buffer intermediate buffer.
     * @param memory_resource memory resource to use for allocating memory for the message.
     * @param receive the receive function; must return true if data are sent, or false if the resource is closed.
     * @return pointer to message or a null ptr if the io_resource is closed.
     */
    template <class Decrypt, class F> message_ptr packet_receive_message(Decrypt&& decrypt, std::vector<char>& buffer, std::pmr::memory_resource& memory_resource, F&& receive) {
        //resize the buffer to hold enough data
        buffer.resize(NETLIB_MAX_MESSAGE_SIZE);

        //receive the data in the buffer
        size_t packet_size;
        if (!receive(buffer.data(), buffer.size(), packet_size)) {
            return nullptr;
        }

        //resize the buffer to the packet size
        buffer.resize(packet_size);

        //decrypt the data
        decrypt(buffer.data(), buffer.size());

        //create the message
        return message_registry::deserialize(buffer, memory_resource);
    }


    ///////////////////////////////////////////////////////////////////////////
    // socket functions
    ///////////////////////////////////////////////////////////////////////////


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
        return stream_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
            return stream_send(buffer, size, socket::nsize, [&](const void* data, const size_t size) {
                return s.send(data, size);
                });
            });
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
        return stream_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void* buffer, const size_t size) {
            return stream_receive(buffer, size, socket::nsize, [&](void* data, size_t size) {
                return s.receive(data, size);
                });
            });
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
        return packet_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
            return s.send(buffer, size) == size;
            });
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
        return packet_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void* buffer, const size_t size, size_t& packet_size) {
            packet_size = s.receive(buffer, size);
            return packet_size != socket::nsize;
            });
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
        return packet_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
            return s.send(buffer, size, addr) == size;
            });
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
        return packet_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void* buffer, const size_t size, size_t& packet_size) {
            packet_size = s.receive(buffer, size, addr);
            return packet_size != socket::nsize;
            });
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
        return stream_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
            return stream_send(buffer, size, pipe::nsize, [&](const void* data, const size_t size) {
                const auto [sent_size, ok] = p.write(data, size);
                return sent_size;
                });
            });
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
        return stream_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void* buffer, size_t size) {
            return stream_receive(buffer, size, pipe::nsize, [&](void* data, size_t size) {
                const auto [received_size, ok] = p.read(data, size);
                return received_size;
                });
            });
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
        //send stream
        if (r.is_stream_oriented()) {
            return stream_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
                return stream_send(buffer, size, ~size_t(0), [&](const void* data, const size_t size) {
                    const auto [sent_size, ok] = r.write(data, size);
                    return ok ? sent_size : ~size_t(0);
                    });
                });
        }

        //send packet
        return packet_send_message(msg, std::forward<Encrypt>(encrypt), buffer, [&](const void* buffer, const size_t size) {
            const auto [sent_size, ok] = r.write(buffer, size);
                return ok;
            });
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
        //stream receive
        if (r.is_stream_oriented()) {
            return stream_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void* buffer, size_t size) {
                return stream_receive(buffer, size, ~size_t(0), [&](void* data, size_t size) {
                    const auto [received_size, ok] = r.read(data, size);
                    return ok ? received_size : ~size_t(0);
                    });
                });
        }

        //packet receive
        return packet_receive_message(std::forward<Decrypt>(decrypt), buffer, memory_resource, [&](void *buffer, const size_t size, size_t& packet_size) {
            const auto [received_size, ok] = r.read(buffer, size);
            packet_size = received_size;
            return ok;
            });
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
