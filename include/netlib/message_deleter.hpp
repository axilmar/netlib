#ifndef NETLIB_MESSAGE_DELETER_HPP
#define NETLIB_MESSAGE_DELETER_HPP


#include <memory_resource>


namespace netlib {


    class message;


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
        message_deleter(std::pmr::memory_resource& memres, size_t size);

        /**
         * Invokes the message's destructor, then deallocates the memory of the message.
         * @param msg message to delete.
         */
        void operator ()(message* msg) const;

    private:
        std::pmr::memory_resource& m_memory_resource;
        size_t m_size;
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_DELETER_HPP
