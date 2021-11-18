#ifndef NETLIB_MESSAGE_POINTER_HPP
#define NETLIB_MESSAGE_POINTER_HPP


#include <memory>
#include "message_deleter.hpp"


namespace netlib {


    class message;


    /**
     * message pointer type.
     */
    template <class T = message>
    using message_pointer = std::unique_ptr<T, message_deleter<T>>;


} //namespace netlib


#endif //NETLIB_MESSAGE_POINTER_HPP
