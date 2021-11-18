#include "netlib/message.hpp"


namespace netlib {


    //The constructor.
    message_deleter::message_deleter(std::pmr::memory_resource& memres, size_t size)
        : m_memory_resource(memres), m_size(size)
    {
    }


    //Invokes the message's destructor, then deallocates the memory of the message.
    void message_deleter::operator ()(message* msg) const {
        msg->~message();
        m_memory_resource.deallocate(msg, m_size);
    }


} //namespace netlib
