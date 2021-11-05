#ifndef NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
#define NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP


#include "encryption_interface.hpp"


namespace netlib {


    /**
     * Encrypted messaging interface.
     * @param Base base class; it can be any class that implements an interface like the class messaging_interface.
     */
    template <class Base> class encrypted_messaging_interface : public Base {
    public:
        /**
         * The default constructor.
         */
        encrypted_messaging_interface() {}

        /**
         * Constructor that initializes the encryption interface this messaging interface uses,
         * and Base with the given arguments.
         * ei encryption interface.
         * @param args arguments.
         * @exception std::invalid_argument thrown if the pointer is null.
         */
        template <class... Args> encrypted_messaging_interface(const encryption_interface_pointer& ei, Args&&... args)
            : Base(std::forward<Args>(args)...)
            , m_encryption_interface(ei)
        {
            check_encryption_interface();
        }

        /**
         * Retrieves the encryption interface.
         * Synchronized method.
         */
        const encryption_interface_pointer& get_encryption_interface() const {
            return m_encryption_interface;
        }

        /**
         * Sets the encryption interface.
         * Synchronized method.
         * @param ei synchronized method.
         * @exception std::invalid_argument thrown if the pointer is null.
         */
        void set_encryption_interface(const encryption_interface_pointer& ei) {
            m_encryption_interface = ei;
            check_encryption_interface();
        }

    protected:
        /**
         * Encrypts, then sends the data.
         * @param buffer buffer with data to transmit.
         * @param send_params send parameters.
         * @return true if the data were sent successfully, false otherwise.
         */
        bool send_data(byte_buffer& buffer, const std::initializer_list<std::any>& send_params) override {
            m_encryption_interface->encrypt(buffer);
            return Base::send_data(buffer, send_params);
        }

        /**
         * Receives the data, then decrypts them.
         * @param buffer buffer to put the data to.
         * @param receive_params receive parameters.
         * @return true if the data were received successfully, false otherwise.
         */
        bool receive_data(byte_buffer& buffer, const std::initializer_list<std::any>& receive_params) override {
            if (Base::receive_data(buffer, receive_params)) {
                m_encryption_interface->decrypt(buffer);
                return true;
            }
            return false;
        }

    private:
        encryption_interface_pointer m_encryption_interface;
        
        void check_encryption_interface() {
            if (!m_encryption_interface) {
                throw std::invalid_argument("Encryption interface pointer shall not be null.");
            }
        }
    };


} //namespace netlib


#endif //NETLIB_ENCRYPTED_MESSAGING_INTERFACE_HPP
