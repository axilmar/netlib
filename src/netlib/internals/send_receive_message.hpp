#ifndef NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP
#define NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP


#include "netlib/message.hpp"
#include "netlib/message_pointer.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/serialization.hpp"


namespace netlib::internals {


    //internal buffer used for temporary actions.
    inline thread_local byte_buffer temp_buffer;


    //Sends a message.
    template <class SendDataFunc>
    bool send_message(const message& msg, SendDataFunc&& send_data) {
        temp_buffer.clear();
        msg.serialize(temp_buffer);
        return send_data(temp_buffer);
    }


    //receives a message.
    template <class ReceiveDataFunc>
    message_pointer<> receive_message(std::pmr::memory_resource& memres, const size_t max_message_size, ReceiveDataFunc&& receive_data) {
        //make room in the temporary buffer
        if (temp_buffer.size() < max_message_size) {
            temp_buffer.resize(max_message_size);
        }

        //receive the data; if the data could not be received, return a null pointer.
        if (!receive_data(temp_buffer)) {
            return message_pointer<>{ nullptr, message_deleter(memres, 0) };
        }

        //peek the message id in order to create the appropriate message from the received id
        message_id id;
        copy_value(&id, reinterpret_cast<const message_id&>(temp_buffer[0]));

        //create a message from the id
        message_pointer<> result = message_registry::create_message(id, memres);

        //deserialize the message
        byte_buffer::position pos{};
        result->deserialize(temp_buffer, pos);

        //return the message
        return result;
    }


} //namespace netlib::internals


#endif //NETLIB_INTERNALS_SEND_RECEIVE_MESSAGE_HPP
