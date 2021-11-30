#ifndef NETLIB_MESSAGE_PTR_HPP
#define NETLIB_MESSAGE_PTR_HPP


#include <memory_resource>
#include <memory>
#include "message_base.hpp"


namespace netlib {


    /**
     * Message deleter for messages.
     */
    class message_deleter {
    public:
        /**
         * The non-initializing constructor.
         */
        message_deleter() {}

        /**
         * Deletes a message.
         */
        void operator ()(message_base* msg) {
            msg->~message_base();
            m_memory_resource->deallocate(msg, m_size);
        }

    private:
        //memory for the message is allocated from this resource
        std::pmr::memory_resource* m_memory_resource;

        //required for the deallocation interface
        size_t m_size;

        //constructor
        message_deleter(std::pmr::memory_resource& memory_resource, const size_t size)
            : m_memory_resource(&memory_resource)
            , m_size(size)
        {
        }

        //only the message registry class can create instances of message_deleter.
        friend class message_registry;
    };


    /**
     * Message pointer.
     */
    using message_ptr = std::unique_ptr<message_base, message_deleter>;


} //namespace netlib


#endif //NETLIB_MESSAGE_PTR_HPP
