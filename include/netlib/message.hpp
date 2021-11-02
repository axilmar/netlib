#ifndef NETLIB_MESSAGE_HPP
#define NETLIB_MESSAGE_HPP


#include "internals/first_field_base.hpp"
#include "message_id.hpp"
#include "message_size.hpp"
#include "field.hpp"


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
        field<message_id, internals::first_field_base> id;

        /**
         * The destructor.
         */
        virtual ~message() {}

        /**
         * Serializes this message.
         * @param buffer destination buffer.
         */
        virtual void serialize(byte_buffer& buffer) const;

        /**
         * Deserializes this message.
         * @param buffer source buffer.
         * @exception message_error thrown if the received message id is different than the message id.
         */
        virtual void deserialize(const byte_buffer& buffer);

        /**
         * Returns a const pointer to the first field, which is the message id.
         * @return pointer to the first field.
         */
        const internals::field_base* get_first_field() const { return &id; }

        /**
         * Returns a pointer to the first field, which is the message id.
         * @return pointer to the first field.
         */
        internals::field_base* get_first_field() { return &id; }

    protected:
        /**
         * The default constructor.
         * @param id message id.
         */
        message(message_id id = 0) : id(id) {}
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_HPP
