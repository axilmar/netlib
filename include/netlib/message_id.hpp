#ifndef NETLIB_MESSAGE_ID_HPP
#define NETLIB_MESSAGE_ID_HPP


#include <cstdint>


/**
 * defines the storage type for message ids.
 */
#ifndef NETLIB_MESSAGE_ID_STORAGE_TYPE
#define NETLIB_MESSAGE_ID_STORAGE_TYPE uint16_t
#endif


/**
 * Defines the number of bits dedicated to namespace index.
 * The rest of the bits are for the message index within the namespace.
 */
#ifndef NETLIB_MESSAGE_ID_NAMESPACE_BITS
#define NETLIB_MESSAGE_ID_NAMESPACE_BITS 6
#endif


namespace netlib {


    /**
     * Message id type. 
     */
    using message_id = NETLIB_MESSAGE_ID_STORAGE_TYPE;


} //namespace netlib


#endif //NETLIB_MESSAGE_ID_HPP
