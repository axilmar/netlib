#ifndef NETLIB_ENDPOINT_HPP
#define NETLIB_ENDPOINT_HPP


#include "message_deleter.hpp"
#include "byte_buffer.hpp"


/**
 * The seed for the random number generator.
 * 
 * If the seed is not defined externally,
 * then it is defined here.
 * It shall be a string.
 * 
 * By default, it is the compilation date + time string.
 */
#ifndef NETLIB_DEFAULT_ENCRYPTION_RANDOM_SEED
#define NETLIB_DEFAULT_ENCRYPTION_RANDOM_SEED __DATE__ " " __TIME__
#endif


namespace netlib {


    /**
     * Base class for endpoints.
     */
    class endpoint : public std::enable_shared_from_this<endpoint> {
    public:
        /**
         * The destructor.
         */
        virtual ~endpoint() {}

        /**
         * Sends a message.
         * It first serializes the given message to a buffer,
         * then encrypts it, then transmits it over the network.
         */
        void send_message(const message& msg);

        /**
         * Waits for data to be received,
         * then decrypts the buffer, 
         * then it creates a message using the message registry,
         * and then it deserializes the created message from the received data.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to message.
         */
        message_pointer receive_message(size_t max_message_size = 4096);

        /**
         * Sets the encryption key.
         * Thread-safe function.
         * @param key the encryption key.
         */
        static void set_encryption_key(const byte_buffer& key);

    protected:
        /**
         * Interface for encrypting the data.
         * The default implementation encrypts the data by the default encryption table.
         * @param buffer buffer of data to encrypt; on return, the encrypted data.
         */
        virtual void encrypt(byte_buffer& buffer);

        /**
         * Interface for decrypting the data.
         * The default implementation decrypts the data by the default encryption table.
         * @param buffer buffer of data to decrypt; on return, the decrypted data.
         */
        virtual void decrypt(byte_buffer& buffer);

        /**
         * Interface for trasmitting the data.
         * @param buffer buffer with data to transmit.
         */
        virtual void send(const byte_buffer& buffer) = 0;

        /**
         * Interface for receiving the data.
         * @param buffer buffer to put the data to.
         */
        virtual void receive(byte_buffer& buffer) = 0;
    };


} //namespace netlib


#endif //NETLIB_ENDPOINT_HPP
