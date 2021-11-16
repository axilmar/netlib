#ifndef NETLIB_MESSAGE_SIZE_HPP
#define NETLIB_MESSAGE_SIZE_HPP


#include <cstdint>


/**
 * Default message size type.
 */
#ifndef NETLIB_MESSAGE_SIZE_TYPE
#define NETLIB_MESSAGE_SIZE_TYPE uint16_t
#endif


namespace netlib {


    /**
     * Message size type.
     */
    using message_size = NETLIB_MESSAGE_SIZE_TYPE;

    
    /**
     * Converts buffer size to message size.
     * @param buffer_size buffer size.
     * @return message size.
     * @exception message_error thrown if the given size value does not fit into message_size.
     */
    message_size buffer_size_to_message_size(const size_t buffer_size);


} //namespace netlib


#endif //NETLIB_MESSAGE_SIZE_HPP
