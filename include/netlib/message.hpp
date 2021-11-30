#ifndef NETLIB_MESSAGE_HPP
#define NETLIB_MESSAGE_HPP


#include <tuple>
#include <exception>
#include "message_base.hpp"
#include "serialization.hpp"
#include "message_registry.hpp"


namespace netlib {


    /**
     * Implementation of a message.
     * @param message message members.
     */
    template <class... T> class message : public message_base, public std::tuple<T...> {

    private:
        //message registration
        struct message_registry_entry {
        public:
            //registers the message
            message_registry_entry() {
                message_registry::register_message<message<T...>>();
            }

            //returns the message id
            netlib::message_id message_id() const {
                static const netlib::message_id id = message_registry::message_id<message<T...>>();
                return id;
            }
        };

        //registers this message
        static inline struct message_registry_entry message_registry_entry;

    public:
        /**
         * the id of the message.
         */
        static inline const netlib::message_id id = message_registry_entry.message_id();

        /**
         * Empty message constructor.
         */
        message() {}

        /**
         * Constructor from arguments.
         * @param args arguments.
         */
        template <class... A> message(A&&... args) : std::tuple<T...>(std::forward<A>(args)...) {}

        /**
         * Retrieves the message id of the message.
         */
        netlib::message_id message_id() const override final { 
            return id;
        }

        /**
         * Serializes this message into the given buffer.
         * @param buffer destination buffer.
         */
        void serialize(std::vector<char>& buffer) const final {
            netlib::serialize(buffer, message_id());
            netlib::serialize(buffer, static_cast<const std::tuple<T...>&>(*this));
        }

        /**
         * Deserializes this message from the given buffer.
         * @param buffer source buffer.
         * @param pos current position into the buffer; on return, the next available position.
         * @exception std::invalid_argument thrown if the deserialized id does not match the id of this message.
         */
        void deserialize(const std::vector<char>& buffer, size_t& pos) final {
            //deserialize the message id
            netlib::message_id id;
            netlib::deserialize(buffer, pos, id);

            //check the message id
            if (id != message_id()) {
                throw std::invalid_argument("Invalid message id");
            }

            //deserialize the arguments
            netlib::deserialize(buffer, pos, static_cast<std::tuple<T...>&>(*this));
        }
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_HPP
