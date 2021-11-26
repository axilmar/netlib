#ifndef NETLIB_UTILITY_HPP
#define NETLIB_UTILITY_HPP


#include <vector>
#include "internet_address.hpp"


namespace netlib {


    /**
     * Returns the current host's name.
     * @return the current host's name.
     * @exception std::runtime_error if the host name could not be retrieved.
     */
    std::string get_host_name();


    /**
     * returns the internet addresses of the given host.
     * @param addr name of host/ip address.
     * @param af address family; if 0, then all address families are considered.
     * @return internet addresses of the given host.
     * @exception std::invalid_argument thrown if the hostname is invalid.
     */
    std::vector<internet_address> get_addresses(const char* addr, int af = 0);


} //namespace netlib


#endif //NETLIB_UTILITY_HPP
