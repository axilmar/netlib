#ifndef NETLIB_SENDING_MESSAGING_INTERFACE_HPP
#define NETLIB_SENDING_MESSAGING_INTERFACE_HPP


#include <any>
#include "message.hpp"


namespace netlib {


    /**
     * Base class for messaging interfaces that send messages. 
     */
    class sending_messaging_interface {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~sending_messaging_interface() {}

        /**
         * Interface for setting the receiver address for the next send_message call.
         * Not all interfaces support this call.
         * @param addr address 
         */
        virtual void set_receiver_address(const std::any& addr) {}

        /**
         * Interface for sending a message.
         * @param msg message to send.
         * @return true if the message was sent, false if it could not be sent.
         */
        virtual bool send_message(message&& msg) = 0;
    };


} //namespace netlib


#endif //NETLIB_SENDING_MESSAGING_INTERFACE_HPP
