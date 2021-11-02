#include "netlib/messaging_interface.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/message_size.hpp"
#include "netlib/stringstream.hpp"


namespace netlib {


    //internal buffers
    static thread_local byte_buffer thread_buffer;


    //global memory resource
    static std::pmr::synchronized_pool_resource global_message_memory_resource;


    //given a buffer, returns the appropriate message size;
    //throws exception if buffer contains too much data.
    static message_size get_message_size(const byte_buffer& buffer) {
        const size_t buffer_size = buffer.size() - sizeof(message_size);

        if (buffer_size <= std::numeric_limits<message_size>::max()) {
            return static_cast<message_size>(buffer_size);
        }

        throw std::runtime_error(stringstream() << "Message is too large; message size = " << buffer_size << "; max message size = " << std::numeric_limits<message_size>::max());
    }


    //Sends a message.
    bool messaging_interface::send_message(const message& msg) {
        //clear the temporary buffer
        thread_buffer.clear();

        //leave room for message size at the buffer start
        serialize(message_size{}, thread_buffer);

        //serialize the message
        msg.serialize(thread_buffer);

        //set the message size at the buffer start
        const message_size msg_size = get_message_size(thread_buffer);
        copy_value(thread_buffer.data(), msg_size);

        //send the message
        return send_message_data(thread_buffer);
    }


    //receives a message.
    message_pointer messaging_interface::receive_message(std::pmr::memory_resource& memres, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_message_data(thread_buffer)) {
            return message_pointer{nullptr, message_deleter(memres, 0)};
        }

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
    message_pointer messaging_interface::receive_message(size_t max_message_size) {
        return messaging_interface::receive_message(global_message_memory_resource, max_message_size);
    }


} //namespace netlib
