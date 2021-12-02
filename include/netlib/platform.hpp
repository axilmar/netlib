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
#define nfds_t ULONG
#endif


#ifdef linux
#define _GNU_SOURCE
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
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


/**
 * Creates a pipe. 
 * @param fds result file descriptors; descriptor 0 is for reading, descriptor 1 is for writing.
 * @param size size of pipe buffer.
 * @return true on success, false on error. 
 */
bool create_pipe(int fds[2], size_t size);


#ifdef _WIN32
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
#endif


#endif //NETLIB_PLATFORM_HPP
