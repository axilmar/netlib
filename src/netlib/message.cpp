#include "netlib/message.hpp"
#include "netlib/message_error.hpp"
#include "netlib/stringstream.hpp"


namespace netlib {


    //Serializes this message.
    void message::serialize(byte_buffer& buffer) const {
        for (const internals::field_base* f = get_first_field(); f; f = f->get_next_field()) {
            f->serialize_this(buffer);
        }
    }


    //Deserializes this message.
    void message::deserialize(const byte_buffer& buffer) {
        byte_buffer::position pos = 0;

        const message_id current_message_id = id;

        //deserialize the id
        internals::field_base* f = get_first_field();
        f->deserialize_this(buffer, pos);

        //check the id; if different, throw exception
        if (id.value != current_message_id) {
            throw message_error(stringstream() << "Received message id " << id.value << " different than current message id " << current_message_id);
        }

        //deserializes the next fields
        for (f = f->get_next_field(); f; f = f->get_next_field()) {
            f->deserialize_this(buffer, pos);
        }
    }


} //namespace netlib
