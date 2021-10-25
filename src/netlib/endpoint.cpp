#include "netlib/endpoint.hpp"
#include "netlib/message_registry.hpp"


namespace netlib {


    //internal buffers
    static byte_buffer thread_buffer;


    //Sends a message.
    void endpoint::send_message(const message& msg) {
        //clear the temporary buffer
        thread_buffer.clear();

        //serialize the message into the temporary buffer
        msg.serialize(thread_buffer);

        //send the data from the temporary buffer
        send(thread_buffer);
    }


    //receives a message.
    message_pointer endpoint::receive_message(std::pmr::memory_resource* memres, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data
        receive(thread_buffer);

        //deserialize the message id
        message_id id;
        size_t pos = 0;
        deserialize(id, thread_buffer, pos);

        //create a message from the id
        message_pointer result = memres ? 
            message_registry::create_message(id, *memres) : 
            message_registry::create_message(id);

        //deserialize the message
        result->deserialize(thread_buffer);

        //return the message
        return result;
    }


} //namespace netlib
