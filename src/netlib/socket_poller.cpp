#include "platform.hpp"
#include "netlib/socket_poller.hpp"
#include "numeric_cast.hpp"


namespace netlib {


    ///////////////////////////////////////////////////////////////////////////
    // PUBLIC
    ///////////////////////////////////////////////////////////////////////////


    //the constructor.
    socket_poller::socket_poller(size_t max_sockets)
        : m_max_sockets(max_sockets + 1) //account for the com socket
        , m_com_socket(ip4::socket_address(ip4::address::loopback, 0))
        , m_com_socket_address(m_com_socket.get_assigned_address())
        , m_entries_changed{}
        , m_stop{}
    {
        //add entry for the internal com socket.
        m_entries.push_back(entry{ &m_com_socket, event_type::read, [](socket_poller&, socket&, event_type) {} });
    }


    //stop polling.
    socket_poller::~socket_poller() {
        {
            std::lock_guard lock(m_mutex);
            m_stop = true;
        }
        m_com_socket.send(temp_byte_buffer(0), m_com_socket_address);
    }


    //add entry.
    bool socket_poller::add(socket& s, event_type e, const event_callback_type& cb) {
        //check the socket
        if (!s) {
            throw std::invalid_argument("Invalid socket.");
        }

        //check the event param
        if (e != event_type::read && e != event_type::write) {
            throw std::invalid_argument("Invalid event type.");
        }

        //check the callback param
        if (!cb) {
            throw std::invalid_argument("Empty event callback.");
        }

        std::lock_guard lock(m_mutex);

        //check the number of entries
        if (m_entries.size() == m_max_sockets) {
            return false;
        }

        //check if the given socket and event is already added
        for (entry& en : m_entries) {
            if (en.socket == &s && en.event == e) {
                throw std::invalid_argument("Socket entry already added.");
            }
        }

        //add the socket
        m_entries.push_back(entry{&s, e, cb});

        //set the entries to have changed
        set_entries_changed();

        //invoke the socket entry added callback
        if (m_on_socket_entry_added) {
            m_on_socket_entry_added(m_entries.size() - 1, s, e, cb);
        }

        //success
        return true;
    }


    //remove entry
    void socket_poller::remove(socket& s, event_type e) {
        std::lock_guard lock(m_mutex);

        //locate the entry
        auto it = m_entries.begin();
        for (; it != m_entries.end(); ++it) {
            entry& en = *it;
            if (en.socket == &s && en.event == e) {
                break;
            }
        }

        //if not found, throw 
        if (it == m_entries.end()) {
            throw std::invalid_argument("Socket entry not found.");
        }

        //keep the entry for invoking the event later
        const entry en = *it;

        //remove the entry
        m_entries.erase(it);

        //set the entries to have changed
        set_entries_changed();

        //invoke the socket entry removed callback
        if (m_on_socket_entry_removed) {
            m_on_socket_entry_removed(m_entries.size() - 1, *en.socket, en.event, en.callback);
        }
    }


    //remove all entries.
    void socket_poller::remove(socket& s) {
        std::lock_guard lock(m_mutex);

        //find and remove entries; iterate backwards so as that index is not invalidated
        size_t remove_count{};
        for (size_t index = m_entries.size(); index > 0; --index) {
            entry& en = m_entries[index - 1];
            if (en.socket == &s) {
                m_entries.erase(m_entries.begin() + index - 1);
                ++remove_count;
            }
        }

        //if no entry was removed, throw
        if (!remove_count) {
            throw std::runtime_error("Socket not found.");
        }

        //set the entries as changed
        set_entries_changed();

        //invoke the socket removed callback
        if (m_on_socket_removed) {
            m_on_socket_removed(m_entries.size() - 1, s);
        }
    }


    //poll.
    socket_poller::poll_status socket_poller::poll(int timeout_ms) {
        static thread_local std::vector<entry> entries;
        static thread_local std::vector<pollfd> pollfds;

        {
            std::lock_guard lock(m_mutex);

            //if stopped
            if (m_stop) {
                return poll_status::stopped;
            }

            //if there are no entries (besides the com socket)
            if (m_entries.size() - 1 == 0) {
                return poll_status::empty;
            }

            //if changed, rebuild the entries/pollfds
            if (m_entries_changed) {
                m_entries_changed = false;

                //make room for new entries
                entries.resize(m_entries.size());
                pollfds.resize(m_entries.size());

                //set entries
                for (size_t i = 0; i < m_entries.size(); ++i) {
                    entries[i] = m_entries[i];
                    pollfds[i].events = m_entries[i].event == event_type::read ? POLLIN : POLLOUT;
                    pollfds[i].fd = m_entries[i].socket->handle();
                }
            }
        }

        //poll
        int poll_result = ::poll(pollfds.data(), numeric_cast<unsigned int>(pollfds.size()), timeout_ms);

        //process events
        if (poll_result > 0) {
            for (size_t i = 0; i < pollfds.size() && poll_result > 0; ++i) {
                if (pollfds[i].revents) {
                    entries[i].callback(*this, *entries[i].socket, entries[i].event);
                    --poll_result;
                }
            }
            return poll_status::success;
        }

        //timeout
        if (poll_result == 0) {
            return poll_status::timeout;
        }

        //error
        throw std::runtime_error(get_last_error_message());
    }


    //Sets the callback that is invoked when a socket entry is added.
    void socket_poller::set_on_socket_entry_added_callback(const std::function<void(const size_t entries_count, socket& s, event_type e, const event_callback_type& cb)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_entry_added = f;
    }


    //Sets the callback that is invoked when a socket entry is removed.
    void socket_poller::set_on_socket_entry_remmoved_callback(const std::function<void(const size_t entries_count, socket& s, event_type e, const event_callback_type& cb)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_entry_removed = f;
    }


    //Sets the callback that is invoked when a socket is removed.
    void socket_poller::set_on_socket_removed_callback(const std::function<void(const size_t entries_count, socket& s)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_removed = f;
    }


    ///////////////////////////////////////////////////////////////////////////
    // PRIVATE
    ///////////////////////////////////////////////////////////////////////////


    //sets the entries as changed
    void socket_poller::set_entries_changed() {
        m_entries_changed = true;
        m_com_socket.send(temp_byte_buffer(0), m_com_socket_address);
    }


} //namespace netlib
