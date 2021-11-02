#ifndef NETLIB_MESSAGE_SIZE_HPP
#define NETLIB_MESSAGE_SIZE_HPP


#include <cstdint>


/**
 * Default message size.
 */
#ifndef NETLIB_MESSAGE_SIZE
#define NETLIB_MESSAGE_SIZE uint16_t
#endif


namespace netlib {


    /**
     * Message size type.
     */
    using message_size = NETLIB_MESSAGE_SIZE;


} //namespace netlib


#endif //NETLIB_MESSAGE_SIZE_HPP
