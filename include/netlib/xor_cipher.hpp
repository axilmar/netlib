#ifndef NETLIB_XOR_ENCRYPTION_INTERFACE_HPP
#define NETLIB_XOR_ENCRYPTION_INTERFACE_HPP


#include <shared_mutex>
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Implements a xor cipher.
     */
    class xor_cipher {
    public:
        /**
         * The key type.
         */
        using key_type = byte_buffer;

        /**
         * The default constructor.
         * It creates a key of the default size, using the function create_random_key().
         */
        xor_cipher();

        /**
         * The copy constructor.
         * @param src source object.
         */
        xor_cipher(const xor_cipher& src);

        /**
         * The move constructor.
         * @param src source object.
         */
        xor_cipher(xor_cipher&& src);

        /**
         * Constructor.
         * @param key key type; the bigger the key, the safer the encryption is.
         */
        xor_cipher(const key_type& key);

        /**
         * Constructor.
         * @param key key type; the bigger the key, the safer the encryption is.
         */
        xor_cipher(key_type&& key);

        /**
         * The copy assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        xor_cipher& operator = (const xor_cipher& src);

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        xor_cipher& operator = (xor_cipher&& src);

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
        void encrypt(byte_buffer& data);

        /**
         * Decrypts the given data using the key given in the constructor.
         * @param data data to decrypt.
         * @param dst destination data with decrypted content.
         */
        void decrypt(byte_buffer& data);

    private:
        //mutex for synchronization
        mutable std::shared_mutex m_mutex;

        //key
        key_type m_key;
    };


} //namespace netlib


#endif //NETLIB_XOR_ENCRYPTION_INTERFACE_HPP
