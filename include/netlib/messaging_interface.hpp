#ifndef NETLIB_MESSAGING_INTERFACE_HPP
#define NETLIB_MESSAGING_INTERFACE_HPP


#include <any>
#include "message_deleter.hpp"
#include "byte_buffer.hpp"


/**
 * defines the size of the biggest packet.
 */
#ifndef NETLIB_MAX_PACKET_SIZE
#define NETLIB_MAX_PACKET_SIZE 4096
#endif


namespace netlib {


    /**
     * Base class for messaging interfaces.
     */
    class messaging_interface : public std::enable_shared_from_this<messaging_interface> {
    public:
        /**
         * The destructor.
         */
        virtual ~messaging_interface() {}

        /**
         * Sends a message.
         * It deserializes the message in a thread-local buffer, then invokes 'send_data'.
         * @param msg message to send.
         * @param send_params send parameters; depends on the underlying implementation.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(const message& msg, const std::initializer_list<std::any>& send_params = std::initializer_list<std::any>{});

        /**
         * Waits for a message. 
         * It receives data by invoking the function 'receive_data', then it deserializes the data into a message.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param receive_params receive parameters; depends on the underlying implementation.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer receive_message(std::pmr::memory_resource& memres, const std::initializer_list<std::any>& receive_params = std::initializer_list<std::any>{}, size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

        /**
         * Waits for a message.
         * Memory for the message is allocated from a global synchronized memory resource.
         * @param receive_params receive parameters; depends on the underlying implementation.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer receive_message(const std::initializer_list<std::any>& receive_params = std::initializer_list<std::any>{}, size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

    protected:
        /**
         * Interface for trasmitting the data.
         * @param buffer buffer with data to transmit.
         * @param send_params send parameters; depends on the underlying implementation.
         * @return true if the data were sent successfully, false otherwise.
         */
        virtual bool send_data(byte_buffer& buffer, const std::initializer_list<std::any>& send_params) = 0;

        /**
         * Interface for receiving the data.
         * @param buffer buffer to put the data to.
         * @param receive_params receive parameters; depends on the underlying implementation.
         * @return true if the data were received successfully, false otherwise.
         */
        virtual bool receive_data(byte_buffer& buffer, const std::initializer_list<std::any>& send_params) = 0;
    };


} //namespace netlib


#endif //NETLIB_MESSAGING_INTERFACE_HPP
