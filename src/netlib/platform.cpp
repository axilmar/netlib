#include "platform.hpp"
#include <stdexcept>
#include <string>


///////////////////////////////////////////////////////////////////////////
// INTERNALS
///////////////////////////////////////////////////////////////////////////


#ifdef _WIN32


static const struct winsock_library {
    winsock_library() {
        WSADATA wsaData;
        const int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (error != NO_ERROR) {
            throw std::runtime_error("Winsock library could not be initialized; error = " + std::to_string(error));
        }
    }

    ~winsock_library() {
        WSACleanup();
    }
} winsock_library;


std::string get_last_error_message() {
    DWORD error_id = WSAGetLastError();
    if (error_id == 0) {
        return std::string();
    }
    LPSTR buffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
    std::string msg_str(buffer, size);
    LocalFree(buffer);
    return'[' + std::to_string(error_id) + "] " + msg_str;
}


std::string get_getaddrinfo_error_message(int error) {
    return get_last_error_message();
}


#elif defined(linux) || defined(__APPLE__)


std::string get_last_error_message() {
    return strerror(errno);
}


std::string get_getaddrinfo_error_message(int error) {
    if (error == EAI_SYSTEM) {
        return get_last_error_message();
    }
    return gai_strerror(error);
}


#endif