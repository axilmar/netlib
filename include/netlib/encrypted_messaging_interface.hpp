#ifndef NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
#define NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP


#include <type_traits>
#include "xor_cipher.hpp"


namespace netlib {


    /**
     * Encrypted messaging interface.
     * @param MessagingInterface base class messaging interface.
     * @param Cipher the cipher to use.
     */
    template <class MessagingInterface, class Cipher = xor_cipher> class encrypted_messaging_interface : public MessagingInterface {
    public:
        /**
         * The default constructor.
         */
        encrypted_messaging_interface() {
        }

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

    protected:
        /**
         * Encrypts, then sends the data.
         * @param buffer buffer with data to transmit.
         * @return true if the data were sent successfully, false otherwise.
         */
        bool send_data(byte_buffer& buffer) override {
            m_cipher.encrypt(buffer);
            return MessagingInterface::send_data(buffer);
        }

        /**
         * Receives the data, then decrypts them.
         * @param buffer buffer to put the data to.
         * @return true if the data were received successfully, false otherwise.
         */
        bool receive_data(byte_buffer& buffer) override {
            if (MessagingInterface::receive_data(buffer)) {
                m_cipher.decrypt(buffer);
                return true;
            }
            return false;
        }

    private:
        //cipher
        Cipher m_cipher;
    };


} //namespace netlib


#endif //NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
