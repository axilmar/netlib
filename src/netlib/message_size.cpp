#include <limits>
#include "netlib/message_size.hpp"
#include "netlib/message_error.hpp"
#include "netlib/stringstream.hpp"


namespace netlib {


    //Converts buffer size to message size.
    message_size buffer_size_to_message_size(const size_t buffer_size) {
        return buffer_size <= std::numeric_limits<message_size>::max() ?
            static_cast<message_size>(buffer_size) :
            throw message_error(stringstream() << "buffer size value " << buffer_size << " does not fit in message_size");
    }


} //namespace netlib
