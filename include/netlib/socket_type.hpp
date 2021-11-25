#ifndef NETLIB_SOCKET_TYPE_HPP
#define NETLIB_SOCKET_TYPE_HPP


namespace netlib {


    /**
     * Socket type.
     */
    enum class socket_type {
        /**
         * stream (i.e. tcp).
         */
        stream,

        /**
         * datagram (i.e. udp).
         */
        datagram
    };


    /**
     * Converts socket type to the current system socket type value.
     * @param st socket type.
     * @return system value for the given socket type.
     * @exception std::invalid_argument thrown if the given socket type is invalid.
     */
    int socket_type_to_system_value(socket_type st);


    /**
     * Converts system value to socket type.
     * @param st socket type system value.
     * @return socket type enum value.
     * @exception std::invalid_argument thrown if the given value is not supported.
     */
    socket_type system_value_to_socket_type(int st);


    /**
     * Converts the socket type to a string.
     * @param st socket type.
     * @return string.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    std::string socket_type_to_string(socket_type st);


    /**
     * Converts a string to an socket type.
     * @param str string.
     * @return socket type.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    socket_type socket_type_from_string(const std::string& str);


} //namespace netlib


#endif //NETLIB_SOCKET_TYPE_HPP
