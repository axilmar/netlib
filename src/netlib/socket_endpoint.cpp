#include "netlib/socket_endpoint.hpp"


namespace netlib {


    //Sends the data with the help of this socket.
    bool socket_endpoint::send_message_data(const byte_buffer& buffer) {
        //stream data
        if (is_streaming_socket()) {

            //send the buffer size
            size_t size = buffer.size();
            switch_endianess(size);
            if (!stream_send(&size, sizeof(size))) {
                return false;
            }

            //send the buffer data
            return stream_send(buffer.data(), buffer.size());
        }

        //send datagram
        return send(buffer) == buffer.size();
    }


    //Receives the data with the help of this socket.
    bool socket_endpoint::receive_message_data(byte_buffer& buffer) {
        //receive streamed data
        if (is_streaming_socket()) {

            //receive buffer size
            size_t size;
            if (!stream_receive(&size, sizeof(size))) {
                return false;
            }
            switch_endianess(size);

            //receive data
            buffer.resize(size);
            return size > 0 ? stream_receive(buffer.data(), buffer.size()) : true;
        }

        //receive datagram

        const size_t received_bytes = receive(buffer);

        if (received_bytes > 0) {
            buffer.resize(received_bytes);
            return true;
        }

        return false;
    }


} //namespace netlib
