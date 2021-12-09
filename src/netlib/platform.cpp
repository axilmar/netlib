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
    return WSAGetLastError();
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


#endif
