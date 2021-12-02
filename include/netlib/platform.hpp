#ifndef NETLIB_PLATFORM_HPP
#define NETLIB_PLATFORM_HPP


#ifdef _WIN32
#pragma warning (disable: 4996)
#include <WinSock2.h>
#include <In6addr.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
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
#include <unistd.h>
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


#ifdef _WIN32


/**
 * Creates a pipe. 
 * @param fds file descriptors; descriptor 0 is for reading, descriptor 1 is for writing.
 * @return 0 on success, -1 on error. 
 */
int pipe(int fds[2]);


#endif _WIN32


#endif //NETLIB_PLATFORM_HPP
