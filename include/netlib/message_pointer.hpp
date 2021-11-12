#ifndef NETLIB_MESSAGE_POINTER_HPP
#define NETLIB_MESSAGE_POINTER_HPP


#include <memory_resource>
#include "message.hpp"


namespace netlib {


    /**
     * Holds a reference to the memory resource that was used for message creation, 
     * in order to return the memory chunk to the resource.
     */
    class message_deleter {
    public:
        /**
         * The constructor.
         * @param memres memory resource.
         * @param size allocation size.
         */
        message_deleter(std::pmr::memory_resource& memres, size_t size)
            : m_memory_resource(memres), m_size(size) {}

        /**
         * Invokes the message's destructor, then deallocates the memory of the message.
         * @param msg message to delete.
         */
        void operator ()(message* msg) const {
            msg->~message();
            m_memory_resource.deallocate(msg, m_size);
        }

    private:
        std::pmr::memory_resource& m_memory_resource;
        size_t m_size;
    };


    /**
     * message pointer type.
     */
    using message_pointer = std::unique_ptr<message, message_deleter>;


} //namespace netlib


#endif //NETLIB_MESSAGE_POINTER_HPP
