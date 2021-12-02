#include "netlib/platform.hpp"
#include <stdexcept>


#ifdef _WIN32


//winsock requires initialization
static const struct winsock_initializer {
    winsock_initializer() {
        WSADATA wsaData;
        const int result = WSAStartup(MAKEWORD(2, 0), &wsaData);
        if (result != 0) {
            throw std::runtime_error("Failed to initialize winsock; error number = " + std::to_string(result));
        }
    }

    ~winsock_initializer() {
        WSACleanup();
    }
} winsock_initializer;


//make error message
static std::string make_error_message(int error_number, const std::string& msg) {
    return std::string("[") + std::to_string(error_number) + std::string("] ") + msg;
}


//Returns the last error number.
int get_last_error_number() {
    return GetLastError();
}


//get last error for windows
std::string get_last_error_message(int error_number) {
    //get the last error number if not given
    if (!error_number) {
        error_number = get_last_error_number();
    }

    //no error exists; return empty string
    if (!error_number) {
        return std::string();
    }

    LPSTR buffer = nullptr;

    //get error message as string
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_number, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);

    //copy it to an std::string
    std::string msg_str(buffer, size);

    //free the buffer
    LocalFree(buffer);

    //create the message
    return make_error_message(error_number, msg_str);
}


//Creates a pipe.
bool create_pipe(int fds[2], size_t size) {
    if (size > std::numeric_limits<unsigned int>::max()) {
        throw std::invalid_argument("Size too big to be stored in a variable of type 'unsigned int'.");
    }
    return _pipe(fds, static_cast<unsigned int>(size), O_BINARY) == 0;
}


#endif //_WIN32


#ifdef linux


#include <errno.h>
#include <string.h>


//Returns the last error number.
int get_last_error_number() {
    return errno;
}


//get last error for linux
std::string get_last_error_message(int error_number) {

    //get the last error if not given
    if (!error_number) {
        error_number = errno;
    }

    //no error exists; return empty string
    if (!error_number) {
        return std::string();
    }

    char buffer[4096];

    //get the error message
    errno = 0;
    int error = strerror_r(error_number, buffer, sizeof(buffer)));

    //if -1 is returned, then the error is stored in errno
    if (error == -1) {
        error = errno;
    }

    //if no error happened, then create the error message
    if (!error) {
        return make_error_message(error_number, buffer);
    }

    //process the error
    switch (error) {
    case EINVAL:
        return make_error_message(EINVAL, "Invalid error number " + std::to_string(error_number));

    case ERANGE:
        return make_error_message(ERANGE, "String buffer too small for error number " + std::to_string(error_number));
    }

    //the error is unknown
    return make_error_message(error, "Unknown error.");
}


//Creates a pipe.
bool create_pipe(int fds[2], size_t size) {
    //check the size
    if (size > std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Size too big to be stored in a variable of type 'int'.");
    }

    //create the pipe
    int error = pipe(fds);

    //there was an error
    if (error) {
        return false;
    }

    //set the pipe size
    int int_size = static_cast<int>(size);
    int capacity = fcntl(fds[0], F_SETPIPE_SZ, int_size);

    //error setting the capacity
    if (capacity != size) {
        close(fd[0]);
        close(fd[1]);
        return false;
    }

    //success
    return true;
}


#endif //linux
