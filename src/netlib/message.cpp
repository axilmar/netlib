#include "netlib/message.hpp"


namespace netlib {


    //Serializes this message.
    void message::serialize(byte_buffer& buffer) const {
        for (const field_base* f = get_first_field(); f; f = f->get_next_field()) {
            f->serialize_this(buffer);
        }
    }


    //Deserializes this message.
    void message::deserialize(const byte_buffer& buffer) {
        byte_buffer::position pos{0};
        for (field_base* f = get_first_field(); f; f = f->get_next_field()) {
            f->deserialize_this(buffer, pos);
        }
    }


} //namespace netlib
