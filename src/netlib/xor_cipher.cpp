#include "netlib/xor_cipher.hpp"
#include "netlib/encryption.hpp"


namespace netlib {


    //apply xor
    template <class T> static void apply_xor(T* data, size_t data_size, const T* key, size_t key_size) {
        for (size_t i = 0; i < data_size; ++i) {
            data[i % data_size] ^= key[i % key_size];
        }
    }


    //The default constructor.
    xor_cipher::xor_cipher()
        : m_key(create_random_key())
    {
    }


    //The copy constructor.
    xor_cipher::xor_cipher(const xor_cipher& src)
        : m_key(src.m_key)
    {
    }


    //The move constructor.
    xor_cipher::xor_cipher(xor_cipher&& src) 
        : m_key(std::move(src.m_key))
    {
    }


    //Constructor.
    xor_cipher::xor_cipher(const key_type& key)
        : m_key(key)
    {
    }


    //Constructor.
    xor_cipher::xor_cipher(key_type&& key)
        : m_key(std::move(key))
    {
    }


    //The copy assignment operator.
    xor_cipher& xor_cipher::operator = (const xor_cipher& src) {
        m_key = src.m_key;
        return *this;
    }


    //The move assignment operator.
    xor_cipher& xor_cipher::operator = (xor_cipher&& src) {
        m_key = std::move(src.m_key);
        return *this;
    }


    //Returns the key.
    xor_cipher::key_type xor_cipher::get_key() const {
        std::shared_lock lock(m_mutex);
        return m_key;
    }


    //Sets the key.
    void xor_cipher::set_key(const key_type& key) {
        std::lock_guard lock(m_mutex);
        m_key = key;
    }


    //Encrypts the given data using the key given in the constructor.
    void xor_cipher::encrypt(byte_buffer& data) {
        std::shared_lock lock(m_mutex);
        apply_xor(data.data(), data.size(), m_key.data(), m_key.size());
    }


    //Decrypts the given data using the key given in the constructor.
    void xor_cipher::decrypt(byte_buffer& data) {
        std::shared_lock lock(m_mutex);
        apply_xor(data.data(), data.size(), m_key.data(), m_key.size());
    }


} //namespace netlib
