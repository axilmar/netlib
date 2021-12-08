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

    //The WSAPoll documentation says: 
    //
    //(https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsapoll)
    //
    //Returns one of the following values.
    //  Return value 	    Description
    //  Zero 	            No sockets were in the queried state before the timer expired.
    //  Greater than zero 	THE NUMBER OF ELEMENTS IN FDARRAY FOR WHICH AN REVENTS MEMBER OF THE POLLFD STRUCTURE IS NONZERO.
    //  SOCKET_ERROR 	    An error occurred.Call the WSAGetLastError function to retrieve the extended error code.
    //
    // However, the above (in capital case) is not 100% accurate: when a socket is invalid, 
    // revents for that socket is set to POLLNVAL, but the return value is SOCKET_ERROR (at least in Windows 10).
    //
    // So, when that happens, we compute the return value ourselves: we iterate the pollfd structures
    // and increment a counter every time we find revents is not zero; and then return that.
    //
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