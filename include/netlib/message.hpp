#ifndef NETLIB_MESSAGE_HPP
#define NETLIB_MESSAGE_HPP


#include <cstdint>
#include "internals/first_field_base.hpp"
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
        field<uint16_t, internals::first_field_base> id;

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
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_HPP
