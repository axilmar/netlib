#ifndef NETLIB_MESSAGE_ID_HPP
#define NETLIB_MESSAGE_ID_HPP


#include <cstdint>


#ifndef NETLIB_MESSAGE_ID_TYPE
#define NETLIB_MESSAGE_ID_TYPE uint16_t
#endif


namespace netlib {


    /**
     * Message id type.
     */
    using message_id = NETLIB_MESSAGE_ID_TYPE;


} //namespace netlib


#endif //NETLIB_MESSAGE_ID_HPP
