#ifndef NETLIB_MESSAGING_INTERFACE_HPP
#define NETLIB_MESSAGING_INTERFACE_HPP


#include "sending_messaging_interface.hpp"
#include "receiving_messaging_interface.hpp"


namespace netlib {


    /**
     * Base class for messaging interfaces.
     * It combines the sending and receiving messaging interfaces.
     */
    class messaging_interface : public sending_messaging_interface, public receiving_messaging_interface {
    public:
    };


} //namespace netlib


#endif //NETLIB_MESSAGING_INTERFACE_HPP
