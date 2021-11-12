#ifndef NETLIB_AUTO_MESSAGE_ID_HPP
#define NETLIB_AUTO_MESSAGE_ID_HPP


#include <typeinfo>
#include "internals/auto_message_id_registry.hpp"
#include "message_registry.hpp"


namespace netlib {


    /**
     * Base class for messages with auto-enumerated id.
     * @param T message type.
     * @param Ref reference type; used for the name of type.
     */
    template <class T, class Ref = T> class auto_message_id {
    public:
        /**
         * Returns the message id for the given message.
         * @return message id.
         */
        static message_id get_message_id() {
            return auto_message_id_entry.get_message_id();
        }

        /**
         * Returns the message id for the given message and registers the message.
         * @return message id.
         */
        static message_id register_message() {
            return auto_message_id_entry.register_message();
        }

    private:
        //internal entry
        struct entry {
            //add entry
            entry() {
                internals::auto_message_id_registry::add(typeid(Ref).name());
            }

            //get message id
            message_id get_message_id() const {
                static const message_id id = internals::auto_message_id_registry::get(typeid(Ref).name());
                return id;
            }

            //get id and register message
            message_id register_message() const {
                const message_id id = get_message_id();
                message_registry::register_message<T>(id);
                return id;
            }
        };

        //global entry
        static inline const entry auto_message_id_entry;
    };


} //namespace netlib


#endif //NETLIB_AUTO_MESSAGE_ID_HPP
