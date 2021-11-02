#ifndef NETLIB_AUTO_MESSAGE_ID_HPP
#define NETLIB_AUTO_MESSAGE_ID_HPP


#include <typeinfo>
#include "internals/auto_message_id_registry.hpp"
#include "message_registry.hpp"


namespace netlib {


    /**
     * Base class for messages with auto-enumerated id.
     * @param T message type.
     */
    template <class T> class auto_message_id {
    public:
        /**
         * Returns the message id for the given message.
         * @return message id.
         */
        static message_id get_message_id() {
            static const message_id id = auto_message_id_entry.register_message();
            return id;
        }

    private:
        //internal entry
        struct entry {
            //add entry
            entry() {
                internals::auto_message_id_registry::add(typeid(T).name());
            }

            //get id and register message
            message_id register_message() const {
                const message_id id = internals::auto_message_id_registry::get(typeid(T).name());
                message_registry::register_message<T>(id);
                return id;
            }
        };

        //global entry
        static const entry auto_message_id_entry;
    };


    //global entry
    template <class T> const typename auto_message_id<T>::entry auto_message_id<T>::auto_message_id_entry;


} //namespace netlib


#endif //NETLIB_AUTO_MESSAGE_ID_HPP
