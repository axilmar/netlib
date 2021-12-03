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
bool create_pipe(uintptr_t fds[2], size_t size) {
    //create a socket
    uintptr_t socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //prepare address to bind the socket to
    sockaddr_in addr{};
    addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    addr.sin_port = 0;

    //bind the socket
    int error = ::bind(socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));

    //bind error
    if (error) {
        throw std::runtime_error(get_last_error_message());
    }

    //success; get address to connect the socket to
    int namelen = sizeof(addr);
    error = getsockname(socket, reinterpret_cast<sockaddr*>(&addr), &namelen);

    //getsockname error
    if (error) {
        throw std::runtime_error(get_last_error_message());
    }

    //connect to same address so as that data can be read
    error = connect(socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));

    //connect error
    if (error) {
        throw std::runtime_error(get_last_error_message());
    }

    //success; set both handles to the socket
    fds[0] = socket;
    fds[1] = socket;
    return true;
}


//Read data from pipe.
int pipe_read(uintptr_t fd, void* buffer, int size, bool& pipe_is_open) {
    int bytes_read = ::recv(fd, reinterpret_cast<char*>(buffer), size, 0);

    if (bytes_read > 0) {
        pipe_is_open = true;
        return bytes_read;
    }

    if (bytes_read == 0) {
        pipe_is_open = is_socket_connected(fd);
        return bytes_read;
    }

    return bytes_read;
}


//Write to pipe.
int pipe_write(uintptr_t fd, const void* buffer, int size, bool& pipe_is_open) {
    int bytes_written = ::send(fd, reinterpret_cast<const char*>(buffer), size, 0);

    if (bytes_written >= 0) {
        pipe_is_open = true;
        return bytes_written;
    }

    pipe_is_open = is_socket_connected(fd);

    return bytes_written;
}


//Closes a pipe.
void close_pipe(uintptr_t fds[2]) {
    closesocket(fds[0]);
    fds[0] = static_cast<uintptr_t>(-1);
    fds[1] = static_cast<uintptr_t>(-1);
}


//poll
int poll(struct pollfd* fds, nfds_t nfds, int timeout) {
    return WSAPoll(fds, nfds, timeout);
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
bool create_pipe(uintptr_t fds[2], size_t size) {
    //check the size
    if (size > std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Size too big to be stored in a variable of type 'int'.");
    }

    int internal_fds[2];

    //create the pipe
    int error = pipe(internal_fds);

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
    fds[0] = internal_fds[0];
    fds[1] = internal_fds[1];
    return true;
}


static void handle_pipe_result(int bytes, bool& pipe_is_open) {
    if (bytes < 0) {
        switch (errno) {
        case EBADF:
        case EPIPE:
            pipe_is_open = false;
            break;
        }
    }
}


//Read data from pipe.
int pipe_read(uintptr_t fd, void* buffer, int size, bool& pipe_is_open) {
    int bytes_read = static_cast<int>(::read(fd, buffer, static_cast<unsigned int>(size)));
    pipe_is_open = true;
    handle_pipe_result(bytes_read, pipe_is_open);
    return bytes_read;
}


//Write to pipe.
int pipe_write(uintptr_t fd, const void* buffer, int size, bool& pipe_is_open) {
    int bytes_written = static_cast<int>(::write(fd, buffer, static_cast<unsigned int>(size)));
    pipe_is_open = true;
    handle_pipe_result(bytes_written, pipe_is_open);
    return bytes_written;
}


//Closes a pipe.
void close_pipe(uintptr_t fds[2]) {
    close(static_cast<int>(fds[0]));
    close(static_cast<int>(fds[1]));
    fds[0] = static_cast<uintptr_t>(-1);
    fds[1] = static_cast<uintptr_t>(-1);
}


#endif //linux


//checks if the socket is still connected.
bool is_socket_connected(uintptr_t handle) {
    //get option
    int optval;
    socklen_t optlen = sizeof(optval);
    int res = getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&optval), &optlen);

    //handle result
    if (res) {
        //handle result on win32
        #ifdef _WIN32
        DWORD error = WSAGetLastError();
        switch (error) {
        case WSAENOTSOCK:
            return false;
        default:
            throw std::runtime_error(get_last_error_message(error));
        }
        #endif

        //handle result on linux
        #ifdef linux
        switch (errno) {
        case EBADF:
        case ENOTSOCK:
            return false;
        default:
            throw std::runtime_error(get_last_error_message(errno));
        }
        #endif
    }

    //handle error value in win32
    #ifdef WIN32
    switch (optval) {
    case WSAENOTCONN:
    case WSAENETRESET:
    case WSAENOTSOCK:
    case WSAESHUTDOWN:
    case WSAECONNABORTED:
    case WSAETIMEDOUT:
    case WSAECONNRESET:
        return false;
    }
    #endif

    //handle error value in linux
    #ifdef linux
    switch (optval) {
    case ECONNRESET:
    case EBADF:
    case ENOTCONN:
    case ENOTSOCK:
    case EPIPE:
        return false;
    }
    #endif

    //socket still connected
    return true;
}


