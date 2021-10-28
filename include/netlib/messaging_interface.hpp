#ifndef NETLIB_MESSAGING_INTERFACE_HPP
#define NETLIB_MESSAGING_INTERFACE_HPP


#include "message_deleter.hpp"
#include "byte_buffer.hpp"


#ifndef NETLIB_MAX_PACKET_SIZE
/**
 * defines the size of the biggest packet. 
 */
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
         * @param msg message to send.
         * @return true if the message was sent, false if it could not be sent.
         */
        bool send_message(const message& msg);

        /**
         * Waits for a message. 
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer receive_message(std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

        /**
         * Waits for a message.
         * Memory for the message is allocated from a global synchronized memory resource.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer receive_message(size_t max_message_size = NETLIB_MAX_PACKET_SIZE);

    protected:
        /**
         * Interface for trasmitting the data.
         * @param buffer buffer with data to transmit.
         * @return true if the data were sent successfully, false otherwise.
         */
        virtual bool send_message_data(const byte_buffer& buffer) = 0;

        /**
         * Interface for receiving the data.
         * @param buffer buffer to put the data to.
         * @return true if the data were received successfully, false otherwise.
         */
        virtual bool receive_message_data(byte_buffer& buffer) = 0;
    };


} //namespace netlib


#endif //NETLIB_MESSAGING_INTERFACE_HPP
