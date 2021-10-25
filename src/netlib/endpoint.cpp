#include <shared_mutex>
#include <random>
#include <limits>
#include "netlib/endpoint.hpp"
#include "netlib/message_registry.hpp"


namespace netlib {


    //encryption table data
    struct encryption_table_data {
        //max encryption key size
        static constexpr size_t DEFAULT_ENCRYPTION_KEY_SIZE = 256;

        //usually the table is read
        std::shared_mutex mutex;

        //the encryption key
        byte_buffer encryption_key;

        //creates the default encryption key
        encryption_table_data() {
            //random engine initialized with hash code from random seed string.
            std::mt19937_64 rde(std::hash<const char*>()(NETLIB_DEFAULT_ENCRYPTION_RANDOM_SEED));

            //distribution is from 0 to std::byte max value.
            std::uniform_int_distribution<int> dist(0, static_cast<int>(std::numeric_limits<byte_buffer::value_type>::max()));

            //prepare the encryption key
            encryption_key.resize(DEFAULT_ENCRYPTION_KEY_SIZE);

            //set the translation table
            for (std::byte& v : encryption_key) {
                v = static_cast<std::byte>(dist(rde));
            }
        }
    };


    //internal buffers
    static byte_buffer thread_buffer;


    //returns the encryption table data
    static encryption_table_data& get_encryption_table_data() {
        static encryption_table_data d;
        return d;
    }


    //xor buffer with another buffer
    static void xor_buffer(byte_buffer& buffer, const byte_buffer& key) {
        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] ^= key[i % key.size()];
        }
    }


    //Sends a message.
    void endpoint::send_message(const message& msg) {
        thread_buffer.clear();
        msg.serialize(thread_buffer);
        encrypt(thread_buffer);
        send(thread_buffer);
    }


    //receives a message.
    message_pointer endpoint::receive_message(size_t max_message_size) {
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }
        receive(thread_buffer);
        decrypt(thread_buffer);
        message_pointer result = message_registry::create_message(reinterpret_cast<const message_id&>(thread_buffer[0]));
        result->deserialize(thread_buffer);
        return result;
    }


    //sets the encryption key.
    void endpoint::set_encryption_key(const byte_buffer& key) {
        encryption_table_data& etd = get_encryption_table_data();
        std::lock_guard lock(etd.mutex);
        etd.encryption_key = key;
    }


    //default data encryption.
    void endpoint::encrypt(byte_buffer& buffer) {
        encryption_table_data& etd = get_encryption_table_data();
        std::shared_lock lock(etd.mutex);
        xor_buffer(buffer, etd.encryption_key);
    }


    //default data decryption.
    void endpoint::decrypt(byte_buffer& buffer) {
        encryption_table_data& etd = get_encryption_table_data();
        std::shared_lock lock(etd.mutex);
        xor_buffer(buffer, etd.encryption_key);
    }


} //namespace netlib
