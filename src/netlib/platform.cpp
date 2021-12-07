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
    if (msg_str.back() == '\n') {
        msg_str.resize(msg_str.size() - 1);
    }
    if (msg_str.back() == '\r') {
        msg_str.resize(msg_str.size() - 1);
    }
    return'[' + std::to_string(error_id) + "] " + msg_str;
}


std::string get_getaddrinfo_error_message(int error) {
    return get_last_error_message();
}


int poll(pollfd* fda, unsigned long fds, int timeout) {
    int result = WSAPoll(fda, fds, timeout);

    if (result >= 0) {
        return result;
    }

    //on linux, all the errors are passed in pollfd.revents;
    //on windows, the WSAPoll function returns -1 if something 
    //is not a socket; therefore, special handling is required.
    DWORD windows_error = WSAGetLastError();
    switch (windows_error) {
        case WSAENOTSOCK:
            result = 0;
            for (size_t i = 0; i < fds; ++i) {
                int error;
                int errorlen = sizeof(error);
                getsockopt(fda[i].fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &errorlen);
                if (error != WSAENOTSOCK) {
                    ++result;
                    fda[i].revents = POLLERR | fda[i].events;
                }
                else {
                    fda[i].revents = 0;
                }
            }
            return result;
    }

    return result;
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