#include "netlib/receiving_messaging_interface.hpp"


namespace netlib {


    //internal global memory pool for allocating memory for received messages.
    static std::pmr::synchronized_pool_resource global_synchronized_memory_resource;


    //Receives a message using a global synchronized memory resource.
    message_pointer receiving_messaging_interface::receive_message(size_t max_message_size) {
        return receive_message(global_synchronized_memory_resource, max_message_size);
    }


} //namespace netlib
