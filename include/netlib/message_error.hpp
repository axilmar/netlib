#ifndef NETLIB_MESSAGE_ERROR_HPP
#define NETLIB_MESSAGE_ERROR_HPP


#include <stdexcept>


namespace netlib {


    /**
     * An error related to messages.
     */
    class message_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };


} //namespace netlib


#endif //NETLIB_MESSAGE_ERROR_HPP
