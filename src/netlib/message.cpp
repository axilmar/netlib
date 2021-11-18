#include "netlib/message.hpp"
#include "netlib/serialization.hpp"
#include "netlib/message_error.hpp"
#include "netlib/stringstream.hpp"
#include "netlib/typeinfo.hpp"


namespace netlib {


    /**
     * The default constructor.
     * @param id message id.
     */
    message::message(message_id id)
        : id(id)
    {
    }


    //Serializes this message.
    void message::serialize(byte_buffer& buffer) const {
        netlib::serialize(buffer, id);
    }


    //Deserializes this message.
    void message::deserialize(const byte_buffer& buffer, byte_buffer::position& pos) {
        message_id temp_id;

        //deserialize the id
        netlib::deserialize(buffer, pos, temp_id);

        //check the id; if different, throw exception
        if (temp_id != id) {
            throw message_error(stringstream() << "Received message id " << temp_id << " different than message id " << id);
        }
    }


    //throws exception
    message_pointer<> message::move(std::pmr::memory_resource& resource) {
        throw std::runtime_error(stringstream() << "Move not implemented for class " << get_typeinfo_name(typeid(*this)));
    }


} //namespace netlib
