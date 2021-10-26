#ifndef NETLIB_SOCKET_ADDRESS_HPP
#define NETLIB_SOCKET_ADDRESS_HPP


#include <cstddef>


namespace netlib {


    class socket;


    class socket_address {
    public:

    private:
        std::byte m_data[64];

        friend class socket;
    };


} //namespace netlib


#endif //NETLIB_SOCKET_ADDRESS_HPP
