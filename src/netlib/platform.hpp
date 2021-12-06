#ifndef NETLIB_PLATFORM_HPP
#define NETLIB_PLATFORM_HPP


//windows
#ifdef _WIN32


#include <WinSock2.h>
#include <ws2tcpip.h>


#undef min
#undef max


//else linux or macos
#elif defined(linux) || defined(__APPLE__)


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>


inline void closesocket(uintptr_t handle) {
    close(static_cast<int>(handle));
}


//else platform not supported
#else


#error "Unsupported platform."


#endif


#include <string>
#include <stdexcept>


std::string get_last_error_message();


std::string get_getaddrinfo_error_message(int error);


template <class F> bool get_address_info(const char* name, F&& func) {
    addrinfo* ai;
    int error = getaddrinfo(name, nullptr, nullptr, &ai);

    if (error) {
        throw std::runtime_error(get_getaddrinfo_error_message(error));
    }

    bool result = false;

    for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
        if (func(tai)) {
            result = true;
            break;
        }
    }

    freeaddrinfo(ai);

    return result;
}


#endif //NETLIB_PLATFORM_HPP
