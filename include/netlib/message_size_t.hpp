#ifndef NETLIB_MESSAGE_SIZE_T_HPP
#define NETLIB_MESSAGE_SIZE_T_HPP


#include <cstdint>


/**
 * Preprocessor definition for message_size_t.
 * By default, it is uint16_t, i.e. the most bytes to transfer is 65536.
 */
#ifndef NETLIB_MESSAGE_SIZE_T
#define NETLIB_MESSAGE_SIZE_T uint16_t
#endif


namespace netlib {


    /**
     * Message size type.
     */
    using message_size_t = NETLIB_MESSAGE_SIZE_T;


} //namespace netlib


#endif //NETLIB_MESSAGE_SIZE_T_HPP
