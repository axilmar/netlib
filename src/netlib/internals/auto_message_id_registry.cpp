#include <string_view>
#include <map>
#include <set>
#include <mutex>
#include "netlib/internals/auto_message_id_registry.hpp"


namespace netlib::internals {


    //internal registry; thread-safe
    class registry {
    public:
        //add entry
        void add(const char* entry) {
            std::lock_guard lock(m_mutex);
            if (!m_ids.empty()) {
                throw std::runtime_error("No new messages can be registered.");
            }
            m_entries.insert(entry);
        }

        //get id for entry
        message_id get(const char* entry) {
            std::lock_guard lock(m_mutex);
            fill_ids();
            return m_ids[entry];
        }

    private:
        std::mutex m_mutex;
        std::set<std::string_view> m_entries;
        std::map<std::string_view, message_id> m_ids;

        //fill the ids map
        void fill_ids() {
            if (m_ids.empty()) {
                message_id index = 0;
                for (const std::string_view& e : m_entries) {
                    m_ids[e] = index;
                    ++index;
                    if (index == 0) {
                        throw std::runtime_error("too many message ids");
                    }
                }
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
    message_id auto_message_id_registry::get(const char* entry) {
        return get_registry().get(entry);
    }


} //namespace netlib::internals
