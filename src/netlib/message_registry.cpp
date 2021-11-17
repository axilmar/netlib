#include <mutex>
#include <vector>
#include "netlib/message_registry.hpp"
#include "internals/message_id.hpp"


namespace netlib {


    //message creation function table
    using message_creation_function_table = std::vector<message_registry::message_creation_function>;


    //namespace table
    using message_creation_namespace_table = std::vector<message_creation_function_table>;


    //internal message registry data
    struct message_registry_data {
        //mutex that protects the data
        std::mutex mutex;

        //vector of message creation functions; id is index into this table
        message_creation_namespace_table message_creation_functions;
    };


    //thread-local table of message creation function
    static thread_local message_creation_namespace_table message_creation_functions;


    //get the message registry data
    static message_registry_data& get_message_registry_data() {
        static message_registry_data mrd;
        return mrd;
    }


    //Registers a message creation function.
    void message_registry::register_message(message_id id, message_creation_function&& func) {
        message_registry_data& mrd = get_message_registry_data();

        //global lock
        std::lock_guard lok(mrd.mutex);

        //get the internal id
        const netlib::internals::message_id& internal_id = reinterpret_cast<const netlib::internals::message_id&>(id);

        //make room in namespaces
        if (internal_id.parts.namespace_index >= mrd.message_creation_functions.size()) {
            mrd.message_creation_functions.resize(internal_id.parts.namespace_index + 1);
        }

        //the table to store the creation function
        message_creation_function_table& msg_table = mrd.message_creation_functions[internal_id.parts.namespace_index];

        //make room in the message table
        if (internal_id.parts.message_index >= msg_table.size()) {
            msg_table.resize(internal_id.parts.message_index + 1);
        }

        //set the function
        msg_table[internal_id.parts.message_index] = std::move(func);
    }


    //create message from id and memory resource.
    message_pointer message_registry::create_message(message_id id, std::pmr::memory_resource& memres) {
        //get the internal id
        const netlib::internals::message_id& internal_id = reinterpret_cast<const netlib::internals::message_id&>(id);

        //try to find the message in the current thread's table
        if (internal_id.parts.namespace_index < message_creation_functions.size()) {
            const message_creation_function_table& msg_table = message_creation_functions[internal_id.parts.namespace_index];
            if (internal_id.parts.message_index < msg_table.size() && msg_table[internal_id.parts.message_index]) {
                return msg_table[internal_id.parts.message_index](memres);
            }
        }

        //not found in the current thread's table; make room in the thread's table
        if (internal_id.parts.namespace_index >= message_creation_functions.size()) {
            message_creation_functions.resize(internal_id.parts.namespace_index + 1);
        }
        message_creation_function_table& msg_table = message_creation_functions[internal_id.parts.namespace_index];
        if (internal_id.parts.message_index >= msg_table.size()) {
            msg_table.resize(internal_id.parts.message_index + 1, nullptr);
        }

        //copy the function from the global table to the local table
        {
            message_registry_data& mrd = get_message_registry_data();
            std::lock_guard lock(mrd.mutex);
            msg_table[internal_id.parts.message_index] = mrd.message_creation_functions[internal_id.parts.namespace_index][internal_id.parts.message_index];
        }

        //create the message
        return msg_table[internal_id.parts.message_index](memres);
    }


} //namespace netlib
