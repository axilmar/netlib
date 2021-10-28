#include "netlib/socket.hpp"


namespace netlib {


    //Does not return until all the data are sent.
    bool socket::stream_send(const void* buffer, size_t size, int flags) {
        const char* data = reinterpret_cast<const char*>(buffer);

        for (;;) {
            //send the data
            const size_t sent_bytes = send(data, size, flags);

            //no data could be sent
            if (sent_bytes == 0) {
                return false;
            }

            //calculate remaining amount of data
            size -= sent_bytes;

            //no remaining amount of data; return true
            if (size == 0) {
                return true;
            }

            //adjust the data pointer by the number of sent bytes
            data += sent_bytes;
        }
    }


    //Does not return until all the data are received.
    bool socket::stream_receive(void* buffer, size_t size, int flags) {
        char* data = reinterpret_cast<char*>(buffer);

        for (;;) {
            //receive the data
            const size_t received_bytes = receive(data, size, flags);

            //no data could be received
            if (received_bytes == 0) {
                return false;
            }

            //calculate remaining amount of data
            size -= received_bytes;

            //no remaining amount of data; return true
            if (size == 0) {
                return true;
            }

            //adjust the data pointer by the number of received bytes
            data += received_bytes;
        }
    }


} //namespace netlib
