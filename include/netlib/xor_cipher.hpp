#ifndef NETLIB_XOR_ENCRYPTION_INTERFACE_HPP
#define NETLIB_XOR_ENCRYPTION_INTERFACE_HPP


#include <shared_mutex>
#include "encryption_interface.hpp"
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Implements a xor cipher.
     */
    class xor_cipher : public encryption_interface {
    public:
        /**
         * The key type.
         */
        using key_type = byte_buffer;

        /**
         * Constructor.
         * @param key key type; the bigger the key, the safer the encryption is.
         */
        xor_cipher(const key_type& key = key_type());

        /**
         * Returns the key.
         * Thread-safe function.
         * @return the key.
         */
        key_type get_key() const;

        /**
         * Sets the key.
         * Thread-safe function.
         * @param key new key.
         */
        void set_key(const key_type& key);

        /**
         * Encrypts the given data using the key given in the constructor.
         * @param data data to encrypt.
         * @param dst destination data with encrypted content.
         */
        void encrypt(byte_buffer& data) override;

        /**
         * Decrypts the given data using the key given in the constructor.
         * @param data data to decrypt.
         * @param dst destination data with decrypted content.
         */
        void decrypt(byte_buffer& data) override;

    private:
        //mutex for synchronization
        mutable std::shared_mutex m_mutex;

        //key
        key_type m_key;
    };


} //namespace netlib


#endif //NETLIB_XOR_ENCRYPTION_INTERFACE_HPP
