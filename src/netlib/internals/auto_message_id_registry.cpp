#include <map>
#include <set>
#include <mutex>
#include <string>
#include "netlib/internals/auto_message_id_registry.hpp"
#include "netlib/typeinfo.hpp"
#include "message_id.hpp"


namespace netlib::internals {


    //internal registry; thread-safe
    class registry {
    public:
        //add entry
        void add(const char* entry) {
            //global synchronization
            std::lock_guard lock(m_mutex);

            //ids already set
            if (!m_ids.empty()) {
                throw std::runtime_error("No new messages can be registered.");
            }

            //get namespace and name
            const auto [ns, name] = split_typeinfo_name(entry);

            //add entry
            m_entries[ns].insert(name);
        }

        //get id for entry
        netlib::message_id get(const char* entry) {
            //global synchronization
            std::lock_guard lock(m_mutex);
            
            //create ids
            enumerate_ids();

            //get namespace and name
            const auto [ns, name] = split_typeinfo_name(entry);

            //get id
            return m_ids[ns][name];
        }

    private:
        std::mutex m_mutex;
        std::map<std::string, std::set<std::string>> m_entries;
        std::map<std::string_view, std::map<std::string, netlib::message_id>> m_ids;

        //enumerate ids
        void enumerate_ids() {
            //if already enumerated, do nothing
            if (!m_ids.empty()) {
                return;
            }

            netlib::message_id namespace_index = 0;

            size_t entry_count = 0;

            //iterate namespaces
            for (const auto& namespace_entry : m_entries) {

                netlib::message_id msg_index = 0;

                //iterate namespace entries
                for (const std::string& name_entry : namespace_entry.second) {

                    //create the id
                    netlib::internals::message_id id;
                    id.parts.namespace_index = namespace_index;
                    id.parts.message_index = msg_index;

                    //check for overflow
                    if (entry_count > 0 && id.value == 0) {
                        throw std::runtime_error("The message_id type does not have enough room for all the messages.");
                    }

                    //set the id
                    m_ids[namespace_entry.first][name_entry] = id.value;

                    //next message
                    ++msg_index;

                    //next entry count
                    ++entry_count;
                }

                //next namespace
                ++namespace_index;
            }
        }
    };


    //get registry
    static registry& get_registry() {
        static registry r;
        return r;
    }


    //add entry
    void auto_message_id_registry::add(const char* entry) {
        get_registry().add(entry);
    }


    //get entry
    netlib::message_id auto_message_id_registry::get(const char* entry) {
        return get_registry().get(entry);
    }


} //namespace netlib::internals
