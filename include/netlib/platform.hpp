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
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
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
bool create_pipe(uintptr_t fds[2], size_t size);


/**
 * Read data from pipe.
 */
int pipe_read(uintptr_t fd, void* buffer, int size, bool& pipe_is_open);


/**
 * Write to pipe.
 */
int pipe_write(uintptr_t fd, const void* buffer, int size, bool& pipe_is_open);


/**
 * Closes a pipe.
 * @param fd file descriptors of the pipe.
 */
void close_pipe(uintptr_t fds[2]);


/**
 * checks if the socket is still connected.
 */
bool is_socket_connected(uintptr_t handle);


#endif //NETLIB_PLATFORM_HPP
