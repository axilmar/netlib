#ifndef NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP
#define NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP


#include "netlib/message.hpp"
#include "netlib/message_pointer.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/serialization.hpp"


namespace netlib::internals {


    //Sends a message.
    template <class SendDataFunc>
    bool send_message(byte_buffer& thread_buffer, const message& msg, SendDataFunc&& send_data) {
        thread_buffer.clear();
        msg.serialize(thread_buffer);
        return send_data(thread_buffer);
    }


    //receives a message.
    template <class ReceiveDataFunc>
    message_pointer<> receive_message(byte_buffer& thread_buffer, std::pmr::memory_resource& memres, const size_t max_message_size, ReceiveDataFunc&& receive_data) {
        //make room in the temporary buffer
        if (thread_buffer.size() < max_message_size) {
            thread_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_data(thread_buffer)) {
            return message_pointer<>{ nullptr, message_deleter(memres, 0) };
        }

        //peek the message id in order to create the appropriate message from the received id
        message_id id;
        copy_value(&id, reinterpret_cast<const message_id&>(thread_buffer[0]));

        //create a message from the id
        message_pointer<> result = message_registry::create_message(id, memres);

        //deserialize the message
        byte_buffer::position pos{};
        result->deserialize(thread_buffer, pos);

        //return the message
        return result;
    }


} //namespace netlib::internals


#endif //NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP
