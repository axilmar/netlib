#ifndef NETLIB_ENDPOINT_HPP
#define NETLIB_ENDPOINT_HPP


#include "message_deleter.hpp"
#include "byte_buffer.hpp"


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
         * @param msg message to send.
         */
        void send_message(const message& msg);

        /**
         * Waits for a message. 
         * @param mesres memory resource to use for allocating memory for the message;
         *  if null, an internal, thread-local resource is used.
         * @param max_message_size maximum number of bytes to receive.
         * @return a pointer to received message.
         */
        message_pointer receive_message(std::pmr::memory_resource* memres = nullptr, size_t max_message_size = 4096);

    protected:
        /**
         * Interface for trasmitting the data.
         * @param buffer buffer with data to transmit.
         */
        virtual void send(byte_buffer& buffer) = 0;

        /**
         * Interface for receiving the data.
         * @param buffer buffer to put the data to.
         */
        virtual void receive(byte_buffer& buffer) = 0;
    };


} //namespace netlib


#endif //NETLIB_ENDPOINT_HPP
