#ifndef NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
#define NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP


#include <type_traits>
#include "xor_cipher.hpp"
#include "netlib/internals/send_receive_message.hpp"


namespace netlib {


    /**
     * Encrypted messaging interface.
     * @param MessagingInterface base class messaging interface.
     * @param Cipher the cipher to use.
     */
    template <class MessagingInterface, class Cipher = xor_cipher> class encrypted_messaging_interface : public MessagingInterface {
    public:
        /**
         * Constructor from MessageInterface arguments.
         * @param args arguments to pass to the MessageInterface constructor.
         */
        template <class... Args, typename = std::enable_if_t<std::is_constructible_v<MessagingInterface, Args...>>>
        explicit encrypted_messaging_interface(Args&&... args)
            : MessagingInterface(std::forward<Args>(args)...)
        {
        }

        /**
         * Constructor from cipher and MessageInterface arguments.
         * @param cipher cipher to use.
         * @param args arguments to pass to the MessageInterface constructor.
         */
        template <class... Args>
        explicit encrypted_messaging_interface(const Cipher& cipher, Args&&... args)
            : MessagingInterface(std::forward<Args>(args)...)
            , m_cipher(cipher)
        {
        }

        /**
         * Constructor from cipher and MessageInterface arguments.
         * @param cipher cipher to use.
         * @param args arguments to pass to the MessageInterface constructor.
         */
        template <class... Args>
        explicit encrypted_messaging_interface(Cipher&& cipher, Args&&... args)
            : MessagingInterface(std::forward<Args>(args)...)
            , m_cipher(std::move(cipher))
        {
        }

        /**
         * Sends a message encrypted.
         * @param msg message to send.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(message&& msg) override {
            return internals::send_message(msg, [&](byte_buffer& buffer) {
                return MessagingInterface::send_packet(buffer, [&](byte_buffer& buffer) {
                    m_cipher.encrypt(buffer);
                    return MessagingInterface::send_data(buffer);
                });
            });
        }

        /**
         * Receives an encrypted message.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) override {
            return internals::receive_message(memres, max_message_size, [&](byte_buffer& buffer) {
                return MessagingInterface::receive_packet(buffer, [&](byte_buffer& buffer) {
                    if (!MessagingInterface::receive_data(buffer)) {
                        return false;
                    }
                    m_cipher.decrypt(buffer);
                    return true;
                });
            });
        }

        using socket_messaging_interface::receive_message;

        /**
         * Sends a message encrypted to a specific socket address.
         * @param msg message to send.
         * @param addr address of receiver.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(message&& msg, const socket_address& addr) {
            return internals::send_message(msg, [&](byte_buffer& buffer) {
                return MessagingInterface::send_packet(buffer, [&](byte_buffer& buffer) {
                    m_cipher.encrypt(buffer);
                    return MessagingInterface::send_data(buffer, addr);
                });
            });
        }

        /**
         * Receives an encrypted message.
         * @param addr address of sender.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(socket_address& addr, std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) {
            return internals::receive_message(memres, max_message_size, [&](byte_buffer& buffer) {
                return MessagingInterface::receive_packet(buffer, [&](byte_buffer& buffer) {
                    if (!MessagingInterface::receive_data(buffer, addr)) {
                        return false;
                    }
                    m_cipher.decrypt(buffer);
                    return true;
                });
            });
        }

        /**
         * Receives an encrypted message.
         * @param addr address of sender.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer<> receive_message(socket_address& addr, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) {
            static std::pmr::synchronized_pool_resource global_memory_pool;
            return receive_message(addr, global_memory_pool, max_message_size);
        }

    private:
        //cipher
        Cipher m_cipher;
    };


} //namespace netlib


#endif //NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
