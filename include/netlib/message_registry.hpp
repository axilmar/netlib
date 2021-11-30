#ifndef NETLIB_MESSAGE_REGISTRY_HPP
#define NETLIB_MESSAGE_REGISTRY_HPP


#include <typeinfo>
#include <vector>
#include "message_ptr.hpp"


namespace netlib {


    /**
     * Global message registry.
     * Thread-safe class.
     */
    class message_registry {
    public:
        //deserialization function type.
        using deserialization_function = message_ptr(*)(const std::vector<char>&, std::pmr::memory_resource&);

        /**
         * Registers the given message type.
         * @param std::runtime_error thrown if the message is already registered or this is called after any call to message_id().
         */
        template <class Msg> static void register_message() {
            register_message(typeid(Msg).name(), message_vtable<Msg>::deserialize);
        }

        /**
         * Returns message id for the given message.
         * @exception std::runtime_error thrown if the message is not registered.
         */
        template <class Msg> static netlib::message_id message_id() {
            return message_id(typeid(Msg).name());
        }

        /**
         * Deserializes the contents of a buffer into a message.
         * @param buffer buffer with data.
         * @param memory_resource memory resource to use for message allocation.
         * @return pointer to message.
         * @exception any thrown from memory_resource::allocate.
         * @exception std::invalid_argument thrown if there is no registered message for the contents of the given buffer.
         */
        static message_ptr deserialize(const std::vector<char>& buffer, std::pmr::memory_resource& memory_resource);

    private:
        //message vtable
        template <class Msg> struct message_vtable {
            //deserialize function
            static message_ptr deserialize(const std::vector<char>& buffer, std::pmr::memory_resource& memory_resource) {
                //allocate message memory
                void* mem = memory_resource.allocate(sizeof(Msg));

                //create message
                message_ptr msg{new(mem) Msg(), message_deleter(memory_resource, sizeof(Msg))};

                //deserialize message
                size_t pos = 0;
                msg->deserialize(buffer, pos);

                //return message
                return msg;
            }
        };

        //registers a message by its name and deserialization function
        static void register_message(const char* msg_name, deserialization_function deserialize);

        //get message id from msg name
        static netlib::message_id message_id(const char* msg_name);
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_REGISTRY_HPP
