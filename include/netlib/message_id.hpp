#ifndef NETLIB_MESSAGE_ID_HPP
#define NETLIB_MESSAGE_ID_HPP


#include <cstdint>


/**
 * Message id type preprocessor definition.
 */
#ifndef NETLIB_MESSAGE_ID
#define NETLIB_MESSAGE_ID uint16_t
#endif


namespace netlib {


    /**
     * Message id type.
     */
    using message_id = NETLIB_MESSAGE_ID;


} //namespace netlib


#endif //NETLIB_MESSAGE_ID_HPP
