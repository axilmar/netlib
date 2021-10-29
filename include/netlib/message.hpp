#ifndef NETLIB_MESSAGE_HPP
#define NETLIB_MESSAGE_HPP


#include <cstdint>
#include "internals/first_field_base.hpp"
#include "field.hpp"


namespace netlib {


    /**
     * Message id type. 
     */
    using message_id = uint16_t;


    /**
     * Base class for messages.
     */
    class message {
    public:
        /**
         * The first field message is always its id,
         * in order to allow the message to be deserialized on the receiver.
         */
        field<message_id, internals::first_field_base> id;

        /**
         * The constructor.
         * @param id message id.
         */
        message(message_id id) : id(id) {}

        /**
         * The destructor.
         */
        virtual ~message() {}

        /**
         * Returns a const pointer to the first field, which is the message id.
         * @return pointer to the first field.
         */
        const field_base* get_first_field() const { return &id; }

        /**
         * Returns a pointer to the first field, which is the message id.
         * @return pointer to the first field.
         */
        field_base* get_first_field() { return &id; }

        /**
         * Serializes this message.
         * @param buffer destination buffer.
         */
        void serialize(byte_buffer& buffer) const;

        /**
         * Deserializes this message.
         * @param buffer source buffer.
         */
        void deserialize(const byte_buffer& buffer);
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_HPP
