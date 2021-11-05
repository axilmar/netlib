#include "netlib/messaging_interface.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/message_size.hpp"
#include "netlib/stringstream.hpp"


namespace netlib {


    //internal buffers
    static thread_local byte_buffer thread_buffer;


    //global memory resource
    static std::pmr::synchronized_pool_resource global_message_memory_resource;


    //Sends a message.
    bool messaging_interface::send_message(const message& msg, const std::initializer_list<std::any>& send_params) {
        //clear the temporary buffer so as that serialized data are written into the buffer from its start
        thread_buffer.clear();

        //serialize the message
        msg.serialize(thread_buffer);

        //send the message
        return send_data(thread_buffer, send_params);
    }


    //receives a message.
    message_pointer messaging_interface::receive_message(std::pmr::memory_resource& memres, const std::initializer_list<std::any>& receive_params, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_data(thread_buffer, receive_params)) {
            return message_pointer{nullptr, message_deleter(memres, 0)};
        }

        //peek the message id in order to create the appropriate message from the received id
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
    message_pointer messaging_interface::receive_message(const std::initializer_list<std::any>& receive_params, size_t max_message_size) {
        return messaging_interface::receive_message(global_message_memory_resource, std::move(receive_params), max_message_size);
    }


} //namespace netlib
