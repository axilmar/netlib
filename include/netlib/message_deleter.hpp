#ifndef NETLIB_MESSAGE_DELETER_HPP
#define NETLIB_MESSAGE_DELETER_HPP


#include <memory_resource>


namespace netlib {


    class message;


    /**
     * Holds a reference to the memory resource that was used for message creation,
     * in order to return the memory chunk to the resource.
     */
    template <class T = message>
    class message_deleter {
    public:
        /**
         * The default constructor.
         */
        message_deleter() {
        }

        /**
         * The constructor.
         * @param memres memory resource.
         * @param size allocation size.
         */
        message_deleter(std::pmr::memory_resource& memres, size_t size)
            : m_memory_resource(&memres)
            , m_size(size)
        {
        }

        /**
         * Invokes the message's destructor, then deallocates the memory of the message.
         * @param msg message to delete.
         */
        void operator ()(T* msg) const {
            msg->~T();
            m_memory_resource->deallocate(msg, m_size);
        }

    private:
        std::pmr::memory_resource* const m_memory_resource;
        const size_t m_size;
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_DELETER_HPP
