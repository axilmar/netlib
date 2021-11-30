#include <shared_mutex>
#include <map>
#include "netlib/message_registry.hpp"
#include "netlib/serialization.hpp"


#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
namespace netlib {
    static std::string demangle_name(const char* name) {
        int status = 0;
        std::unique_ptr<char, void(*)(void*)> res{ abi::__cxa_demangle(name, NULL, NULL, &status), std::free };
        return (status == 0) ? res.get() : name;
    }
}
#else
namespace netlib {
    std::string demangle_name(const char* name) {
        return name;
    }
}
#endif


namespace netlib {


    //message registry data
    struct message_registry_data {
        //mutex for global protection
        std::shared_mutex mutex;

        //name to message id
        std::map<std::string, message_id> name_to_id;

        //message name to message deserialization function
        std::map<std::string, message_registry::deserialization_function> name_to_deserialization_function;

        //message id to message deserialization function
        std::map<message_id, message_registry::deserialization_function> id_to_deserialization_function;
    };


    //get the global data
    static message_registry_data& get_message_registry_data() {
        static message_registry_data d;
        return d;
    }


    //deserialize message
    message_ptr message_registry::deserialize(const std::vector<char>& buffer, std::pmr::memory_resource& memory_resource) {
        message_registry_data& data = get_message_registry_data();

        //get the message id stored in the beginning of the buffer
        netlib::message_id id;
        size_t pos = 0;
        netlib::deserialize(buffer, pos, id);

        //global lock in order to find the deserialization function; since it's shared, 
        //it will not stop threads from reading the data concurrently
        deserialization_function func;
        {
            std::shared_lock lock(data.mutex);

            //find the id
            auto it = data.id_to_deserialization_function.find(id);

            //if found, get the deserialization function
            if (it != data.id_to_deserialization_function.end()) {
                func = it->second;
            }

            //else not found, throw exception
            else {
                throw std::runtime_error("No deserialization function found for id = " + std::to_string(id) + '.');
            }
        }

        //deserialize the message
        return func(buffer, memory_resource);
    }


    //registers a message by its name and deserialization function
    void message_registry::register_message(const char* msg_name, deserialization_function deserialize) {
        message_registry_data& data = get_message_registry_data();

        //global write lock; contention does not matter, 
        //this will happen once per message before main(), if using message<T...>.
        std::lock_guard lock(data.mutex);

        //if the messages ids are already created, throw exception
        if (!data.id_to_deserialization_function.empty()) {
            throw std::runtime_error("A message cannot be registered after the messages are enumerated.");
        }

        //use demangled names
        const std::string dm_name = demangle_name(msg_name);

        //insert the entry
        auto [it, ok] = data.name_to_id.emplace(dm_name, 0);

        //if insertion was not possible, throw
        if (!ok) {
            throw std::runtime_error("Message " + dm_name + " already registered.");
        }

        //add the deserialization function
        data.name_to_deserialization_function.emplace(dm_name, deserialize);
    }


    //get message id from msg name
    netlib::message_id message_registry::message_id(const char* msg_name) {
        message_registry_data& data = get_message_registry_data();

        //use demangled names
        const std::string dm_name = demangle_name(msg_name);

        //global write lock; contention does not matter, 
        //this will happen once per message, if using message<T...>.
        std::lock_guard lock(data.mutex);

        //if the messages are not enumerated, enumerate them now
        if (data.id_to_deserialization_function.empty()) {
            netlib::message_id id = 0;

            //enumerate the messages lexicographically
            for (auto& entry : data.name_to_id) {
                //set the id of the entry
                entry.second = id;

                //add deserialization function entry
                data.id_to_deserialization_function.emplace(id, data.name_to_deserialization_function[dm_name]);

                //next message id
                ++id;

                //if the id goes back to 0, then a bigger type is needed
                if (id == 0) {
                    throw std::runtime_error("Message id type too small for the number of registered messages.");
                }
            }
        }

        //find the entry
        auto it = data.name_to_id.find(dm_name);

        //if found, return the id
        if (it != data.name_to_id.end()) {
            return it->second;
        }

        //not found
        throw std::runtime_error("Unknown message " + dm_name + '.');
    }


} //namespace netlib
