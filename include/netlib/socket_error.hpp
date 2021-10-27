#ifndef NETLIB_SOCKET_ERROR_HPP
#define NETLIB_SOCKET_ERROR_HPP


#include <stdexcept>


namespace netlib {


    /**
     * Exception thrown when a socket-related function fails.
     */
    class socket_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_ERROR_HPP
