#ifndef NETLIB_RECEIVING_MESSAGING_INTERFACE_HPP
#define NETLIB_RECEIVING_MESSAGING_INTERFACE_HPP


#include <memory_resource>
#include "message_pointer.hpp"
#include "max_packet_size.hpp"


namespace netlib {


    /**
     * Base class for messaging interfaces that receive messages. 
     */
    class receiving_messaging_interface {
    public:
        /**
         * Virtual destructor due to polymorphism. 
         */
        virtual ~receiving_messaging_interface() {}

        /**
         * Interface for receiving a message.
         * @param mesres memory resource to use for allocating memory for the message.
         * @param max_message_size maximum number of bytes that can be possibly received.
         * @return a pointer to the received message or null if reception was impossible.
         */
        virtual message_pointer receive_message(std::pmr::memory_resource& memres, size_t max_message_size = NETLIB_MAX_PACKET_SIZE) = 0;

        /**
         * Receives a message using a global synchronized memory resource.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to the received message or null if reception was impossible.
         */
        message_pointer receive_message(size_t max_message_size = NETLIB_MAX_PACKET_SIZE);
    };


} //namespace netlib


#endif //NETLIB_RECEIVING_MESSAGING_INTERFACE_HPP
