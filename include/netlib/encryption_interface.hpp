#ifndef NETLIB_ENCRYPTION_INTERFACE_HPP
#define NETLIB_ENCRYPTION_INTERFACE_HPP


#include <memory>
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Base class for encryption interfaces.
     */
    class encryption_interface : public std::enable_shared_from_this<encryption_interface> {
    public:
        /**
         * Interface for encrypting data.
         * @param data data to encrypt.
         * @param dst destination data with encrypted content.
         */
        virtual void encrypt(byte_buffer& data) = 0;

        /**
         * Interface for decrypting data.
         * @param data data to decrypt.
         * @param dst destination data with decrypted content.
         */
        virtual void decrypt(byte_buffer& data) = 0;
    };


    /**
     * Encryption interface pointer.
     */
    using encryption_interface_pointer = std::shared_ptr<encryption_interface>;


} //namespace netlib


#endif //NETLIB_ENCRYPTION_INTERFACE_HPP
