#ifndef NETLIB_PLATFORM_HPP
#define NETLIB_PLATFORM_HPP


#ifdef _WIN32
#include <WinSock2.h>
#include <In6addr.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#undef min
#undef max
#endif


#ifdef linux
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif


#include <string>


/**
 * Retrieves the last error as a string.
 * @param error_number error number; if 0, then it is retrieved from platform error number.
 */
std::string get_last_error(int error_number = 0);


#endif //NETLIB_PLATFORM_HPP
