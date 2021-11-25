#ifndef NETLIB_PROTOCOL_HPP
#define NETLIB_PROTOCOL_HPP


namespace netlib {


    /**
     * protocol.
     */
    enum protocol {
        /**
         * tcp.
         */
        tcp,

        /**
         * udp.
         */
        udp
    };


    /**
     * Converts protocol current system protocol value.
     * @param p protocol enum value.
     * @return system value for protocol.
     * @exception std::invalid_argument thrown if the given protocol is invalid.
     */
    int protocol_to_system_value(protocol p);


    /**
     * Converts system value to protocol.
     * @param p protocol system value.
     * @return address value enumeration.
     * @exception std::invalid_argument thrown if the given value is not supported.
     */
    protocol system_value_to_protocol(int p);


    /**
     * Converts the protocol to a string.
     * @param p protocol.
     * @return string.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    std::string protocol_to_string(protocol p);


    /**
     * Converts a string to an protocol.
     * @param str string.
     * @return protocol.
     * @exception std::invalid_argument thrown if the input value is invalid.
     */
    protocol protocol_from_string(const std::string& str);


} //namespace netlib


#endif //NETLIB_PROTOCOL_HPP
