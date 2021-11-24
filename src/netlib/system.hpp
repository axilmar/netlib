#ifndef NETLIB_SYSTEM_HPP
#define NETLIB_SYSTEM_HPP


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


//get last error message.
std::string get_last_error(int error_number = 0);


#endif //NETLIB_SYSTEM_HPP
