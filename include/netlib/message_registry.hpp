#ifndef NETLIB_MESSAGE_REGISTRY_HPP
#define NETLIB_MESSAGE_REGISTRY_HPP


#include "message_deleter.hpp"
#include "message_error.hpp"


namespace netlib {


    /**
     * Global message registry.
     */
    class message_registry {
    public:

        /**
         * Type of message creation function.
         * @param memres memory resource.
         * @return a pointer to message.
         */
        using message_creation_function = message_pointer (*)(std::pmr::memory_resource& memres);

        /**
         * Registers a message creation function.
         * Thread-safe function.
         * @param id id of message.
         * @param func message function.
         */
        static void register_message(message_id id, message_creation_function&& func);

        /**
         * Creates the appropriate message object, depending on message id.
         * Thread-safe function.
         * @param id message id.
         * @param memres memory resource to use for allocating memory for the message.
         * @return pointer to created message.
         * @exception message_error thrown if there was no registration found for the given message id.
         */
        static message_pointer create_message(message_id id, std::pmr::memory_resource& memres);
    };


    /**
     * Message registration.
     * @param T message type.
     */
    template <class T> class message_registration {
    public:
        /**
         * Declares a message registration for the given message.
         */
        message_registration() {
            message_registry::register_message(T::ID, &create_message);
        }

    private:
        //the message creation function.
        static message_pointer create_message(std::pmr::memory_resource& memres) {
            void* mem = memres.allocate(sizeof(T));
            return { new (mem) T(), message_deleter(memres, sizeof(T)) };
        }
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_REGISTRY_HPP
