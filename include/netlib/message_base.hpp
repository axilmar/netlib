#ifndef NETLIB_MESSAGE_BASE_HPP
#define NETLIB_MESSAGE_BASE_HPP


#include <vector>
#include "message_id.hpp"


namespace netlib {


    /**
     * Base class for messages.
     */
    class message_base {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~message_base() {}

        /**
         * Interface for retrieving the message id of the message.
         */
        virtual message_id message_id() const = 0;

        /**
         * Serializes this message into the given buffer.
         * @param buffer destination buffer.
         */
        virtual void serialize(std::vector<char>& buffer) const = 0;

        /**
         * Deserializes this message from the given buffer.
         * @param buffer source buffer.
         * @param pos current position into the buffer; on return, the next available position.
         */
        virtual void deserialize(const std::vector<char>& buffer, size_t& pos) = 0;
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_BASE_HPP
