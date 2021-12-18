#include "platform.hpp"
#include <stdexcept>
#include <string>


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


int get_last_error_number() {
    const int error = GetLastError();
    return error;
}


std::string get_error_message(int error) {
    if (!error) {
        return std::string();
    }
    LPSTR buffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
    std::string msg_str(buffer, size);
    LocalFree(buffer);
    if (msg_str.back() == '\n') {
        msg_str.resize(msg_str.size() - 1);
    }
    if (msg_str.back() == '\r') {
        msg_str.resize(msg_str.size() - 1);
    }
    return'[' + std::to_string(error) + "] " + msg_str;
}


std::string get_last_error_message() {
    return get_error_message(get_last_error_number());
}


bool is_socket_closed_error(int error) {
    switch (error) {
    case WSAECONNABORTED:
    case WSAECONNRESET:
    case WSAENOTSOCK:
        return true;
    }
    return false;
}


int poll(pollfd* fda, unsigned long fds, int timeout) {
    int result = WSAPoll(fda, fds, timeout);

    if (result >= 0) {
        return result;
    }

    DWORD windows_error = WSAGetLastError();
    switch (windows_error) {
    case WSAENOTSOCK:
        result = 0;
        for (size_t i = 0; i < fds; ++i) {
            if (fda[i].revents) {
                ++result;
            }
        }
        return result;
    }

    return result;
}


int get_connection_timeout_error_number() {
    return WSAETIMEDOUT;
}


int get_socket_closed_error_number() {
    return WSAENOTSOCK;
}


#endif
