#ifndef NETLIB_MESSAGE_HPP
#define NETLIB_MESSAGE_HPP


#include "message_id.hpp"
#include "byte_buffer.hpp"
#include "message_pointer.hpp"


namespace netlib {


    /**
     * Base class for messages.
     */
    class message {
    public:
        /**
         * The first field message is always its id,
         * in order to allow the message to be deserialized on the receiver.
         */
        const message_id id;

        /**
         * The destructor.
         */
        virtual ~message() {}

        /**
         * Serializes this message.
         * The default implementation serializes the id.
         * @param buffer destination buffer.
         */
        virtual void serialize(byte_buffer& buffer) const;

        /**
         * Deserializes this message.
         * The default implementation deserializes the id and compares it to the message id.
         * @param buffer source buffer.
         * @param pos source position.
         * @exception message_error thrown if the received message id is different than the message id.
         */
        virtual void deserialize(const byte_buffer& buffer, byte_buffer::position& pos);

        /**
         * Interface for moving the object to a specific memory resource.
         * Usually, messages are created on the stack and passed around as lvalue references,
         * but a messaging interface implementation might enqueue a message;
         * this call moves the message to a heap-allocated object,
         * so as that it can be stored into a queue.
         * The default implementation throws std::runtime_error because it is not implemented.
         * @param resource memory resource to allocate the message memory for.
         * @return pointer to message allocated on the memory resource. 
         */
        virtual message_pointer move(std::pmr::memory_resource& resource);

    protected:
        /**
         * The default constructor.
         * @param id message id.
         */
        message(message_id id = 0);
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_HPP
