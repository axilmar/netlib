#include <mutex>
#include <vector>
#include "netlib/message_registry.hpp"


namespace netlib {


    //message creation function table
    using message_creation_function_table = std::vector<message_registry::message_creation_function>;


    //internal message registry data
    struct message_registry_data {
        //mutex that protects the data
        std::mutex mutex;

        //vector of message creation functions; id is index into this table
        message_creation_function_table message_creation_functions;
    };


    //thread-local table of message creation function
    static thread_local message_creation_function_table message_creation_functions;


    //local thread memory resource
    static thread_local std::pmr::unsynchronized_pool_resource thread_memory_pool;


    //get the message registry data
    static message_registry_data& get_message_registry_data() {
        static message_registry_data mrd;
        return mrd;
    }


    //the default message creation function throws message_error on the message id
    static message_pointer invalid_message_creation_function(std::pmr::memory_resource& memres) {
        throw message_error("id");
    }


    //Registers a message creation function.
    void message_registry::register_message(message_id id, message_creation_function&& func) {
        message_registry_data& mrd = get_message_registry_data();

        //global lock
        std::lock_guard lok(mrd.mutex);

        //make space for the given id
        if (id >= mrd.message_creation_functions.size()) {
            mrd.message_creation_functions.resize(id + 1, invalid_message_creation_function);
        }

        //set the function
        mrd.message_creation_functions[id] = std::move(func);
    }


    //create message from id and memory resource.
    message_pointer message_registry::create_message(message_id id, std::pmr::memory_resource& memres) {
        //create function from thread-local table
        if (id < message_creation_functions.size() && message_creation_functions[id]) {
            return message_creation_functions[id](memres);
        }

        //make room in the local table for the function
        if (id >= message_creation_functions.size()) {
            message_creation_functions.resize(id + 1, invalid_message_creation_function);
        }

        //copy the function to the local table
        {
            message_registry_data& mrd = get_message_registry_data();
            std::lock_guard lock(mrd.mutex);
            message_creation_functions[id] = mrd.message_creation_functions[id];
        }

        //create the message
        return message_creation_functions[id](memres);
    }


    //create message from message id.
    message_pointer message_registry::create_message(message_id id) {
        return create_message(id, thread_memory_pool);
    }


} //namespace netlib
