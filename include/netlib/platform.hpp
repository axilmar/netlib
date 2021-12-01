#ifndef NETLIB_PLATFORM_HPP
#define NETLIB_PLATFORM_HPP


#ifdef _WIN32
#include <WinSock2.h>
#include <In6addr.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>
#include <Windows.h>
#undef min
#undef max
#define poll WSAPoll
#define nfds_t ULONG
#endif


#ifdef linux
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#endif


#include <string>


/**
 * Returns the last error number.
 */
int get_last_error_number();


/**
 * Retrieves the last error as a string.
 * @param error_number error number; if 0, then it is retrieved from platform error number.
 */
std::string get_last_error_message(int error_number = 0);


#endif //NETLIB_PLATFORM_HPP
