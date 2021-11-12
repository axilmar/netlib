#include "netlib/messaging_interface.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/message_size.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/serialization.hpp"


namespace netlib {


    //internal buffers
    static thread_local byte_buffer thread_buffer;


    //global memory resource
    static std::pmr::synchronized_pool_resource global_message_memory_resource;


    //Sends a message.
    bool messaging_interface::send_message(const message& msg) {
        thread_buffer.clear();
        msg.serialize(thread_buffer);
        return send_data(thread_buffer);
    }


    //receives a message.
    message_pointer messaging_interface::receive_message(std::pmr::memory_resource& memres, size_t max_message_size) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_data(thread_buffer)) {
            return message_pointer{nullptr, message_deleter(memres, 0)};
        }

        //peek the message id in order to create the appropriate message from the received id
        message_id id;
        copy_value(&id, reinterpret_cast<const message_id&>(thread_buffer[0]));

        //create a message from the id
        message_pointer result = message_registry::create_message(id, memres);

        //deserialize the message
        byte_buffer::position pos{ 0 };
        result->deserialize(thread_buffer, pos);

        //return the message
        return result;
    }


    //receives for a message.
    message_pointer messaging_interface::receive_message(size_t max_message_size) {
        return messaging_interface::receive_message(global_message_memory_resource, max_message_size);
    }


} //namespace netlib
