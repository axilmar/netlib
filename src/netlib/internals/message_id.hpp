#ifndef NETLIB_INTERNALS_MESSAGE_ID_HPP
#define NETLIB_INTERNALS_MESSAGE_ID_HPP


#include <climits>
#include "netlib/message_id.hpp"


namespace netlib::internals {


    //internal definition of message id.
    union message_id {
        //complete value
        netlib::message_id value;

        //parts
        struct parts {
            //message index
            netlib::message_id message_index : (sizeof(netlib::message_id) * CHAR_BIT) - NETLIB_MESSAGE_ID_NAMESPACE_BITS;

            //namespace index
            netlib::message_id namespace_index : NETLIB_MESSAGE_ID_NAMESPACE_BITS;
        } parts;
    };


    static_assert(sizeof(message_id::parts) == sizeof(netlib::message_id), "Internal message id size different from external message id size.");


} //namespace netlib::internals


#endif //NETLIB_INTERNALS_MESSAGE_ID_HPP
