#ifndef NETLIB_MESSAGE_POINTER_HPP
#define NETLIB_MESSAGE_POINTER_HPP


#include <memory>
#include "message_deleter.hpp"


namespace netlib {


    class message;


    /**
     * message pointer type.
     */
    using message_pointer = std::unique_ptr<message, message_deleter>;


} //namespace netlib


#endif //NETLIB_MESSAGE_POINTER_HPP
