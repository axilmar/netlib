#include "netlib/endpoint.hpp"
#include "netlib/message_registry.hpp"


namespace netlib {


    //internal buffers
    static byte_buffer thread_buffer;


    //global memory resource
    static std::pmr::synchronized_pool_resource global_message_memory_resource;


    //Sends a message.
    void endpoint::send_message(const message& msg) {
        //clear the temporary buffer
        thread_buffer.clear();

        //serialize the message
        msg.serialize(thread_buffer);

        //send the message
        send_message_data(thread_buffer);
    }


    //receives a message.
    message_pointer endpoint::receive_message(std::pmr::memory_resource& memres, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data
        receive_message_data(thread_buffer);

        //peek the message id
        message_id id;
        copy_value(&id, reinterpret_cast<const message_id&>(thread_buffer[0]));

        //create a message from the id
        message_pointer result = message_registry::create_message(id, memres);

        //deserialize the message
        result->deserialize(thread_buffer);

        //return the message
        return result;
    }


    /**
     * Waits for a message.
     * Memory for the message is allocated from a global synchronized memory resource.
     * @param max_message_size maximum number of bytes to receive.
     * @return a pointer to received message.
     */
    message_pointer endpoint::receive_message(size_t max_message_size) {
        return endpoint::receive_message(global_message_memory_resource, max_message_size);
    }


} //namespace netlib
