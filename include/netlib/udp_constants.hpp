#ifndef NETLIB_UDP_CONSTANTS_HPP
#define NETLIB_UDP_CONSTANTS_HPP


namespace netlib {


    /**
     * Max packet size for udp. 
     * The value 65507 is derived from the following formula: 
     * 65535 - (8 bytes for udp header) - (20 bytes for ip header) = 65507.
     * More information at: https://en.wikipedia.org/wiki/User_Datagram_Protocol.
     */
    static constexpr size_t udp_max_packet_size = 65507;


} //namespace netlib


#endif //NETLIB_UDP_CONSTANTS_HPP
