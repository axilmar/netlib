#ifndef NETLIB_PLATFORM_HPP
#define NETLIB_PLATFORM_HPP


#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#undef min
#undef max
#define HOST_NAME_MAX 256
#include <string>
int get_last_error_number();
std::string get_error_message(int error);
std::string get_last_error_message();
bool is_socket_closed_error(int error);
#endif


#endif //NETLIB_PLATFORM_HPP
