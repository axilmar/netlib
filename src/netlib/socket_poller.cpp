#include "platform.hpp"
#include "netlib/socket_poller.hpp"
#include "numeric_cast.hpp"


namespace netlib {


    //manages the poll counter.
    class poll_counter_manager {
    public:
        //constructor; throw std::logic_error if counter is to become 2.
        poll_counter_manager(std::atomic<size_t>& counter) : m_counter(counter) {
            //check the counter; if it is to become 2, throw
            if (m_counter.fetch_add(1, std::memory_order_acquire) == 1) {

                //put the value back to what it was
                m_counter.fetch_sub(1, std::memory_order_relaxed);

                //throw exception
                throw std::logic_error("Cannot poll by multiple threads.");
            }
        }

        //decrements the counter.
        ~poll_counter_manager() {
            m_counter.fetch_sub(1, std::memory_order_relaxed);
        }

    private:
        //counter
        std::atomic<size_t>& m_counter;
    };


    //the constructor.
    socket_poller::socket_poller(size_t max_sockets)
        : m_max_sockets(max_sockets + 1) //account for the com socket
        , m_com_socket(std::make_shared<udp::server_socket>(socket_address(ip_address::ip4::loopback, 0)))
        , m_com_socket_address(m_com_socket->bound_address())
        , m_entries_changed{}
        , m_stop{}
        , m_poll_counter{0}
    {
        //add entry for the internal com socket.
        m_entries.push_back(entry{ m_com_socket, event_type::read, [](socket_poller&, const socket_ptr& s, event_type, status_flags) {
            std::vector<char> buffer;
            socket_address src;
            std::static_pointer_cast<udp::server_socket>(s)->receive(buffer, src, 0);
            } });
    }


    //stop polling.
    socket_poller::~socket_poller() {
        stop();
    }


    //add entry.
    bool socket_poller::add(const socket_ptr& s, event_type e, const event_callback_type& cb) {
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
            if (en.socket == s && en.event == e) {
                throw std::invalid_argument("Socket entry already added.");
            }
        }

        //add the socket
        m_entries.push_back(entry{s, e, cb});

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
    void socket_poller::remove(const socket_ptr& s, event_type e) {
        std::lock_guard lock(m_mutex);

        //locate the entry
        auto it = m_entries.begin();
        for (; it != m_entries.end(); ++it) {
            entry& en = *it;
            if (en.socket == s && en.event == e) {
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
            m_on_socket_entry_removed(m_entries.size() - 1, en.socket, en.event, en.callback);
        }
    }


    //remove all entries.
    void socket_poller::remove(const socket_ptr& s) {
        std::lock_guard lock(m_mutex);

        //find and remove entries; iterate backwards so as that index is not invalidated
        size_t remove_count{};
        for (size_t index = m_entries.size(); index > 0; --index) {
            entry& en = m_entries[index - 1];
            if (en.socket == s) {
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
        //use RAII to manage poll counter increments
        poll_counter_manager manage_poll_counter(m_poll_counter);

        {
            std::lock_guard lock(m_mutex);

            //if stopped
            if (m_stop) {
                return poll_status::stopped;
            }

            //if there are no m_poll_entries (besides the com socket)
            if (m_entries.size() - 1 == 0) {
                return poll_status::empty;
            }

            //if changed, rebuild the m_poll_entries/m_poll_fds
            if (m_entries_changed) {
                m_entries_changed = false;

                //make room for new m_poll_entries
                m_poll_entries.resize(m_entries.size());
                m_poll_fds.resize(m_entries.size());

                //set m_poll_entries
                for (size_t i = 0; i < m_entries.size(); ++i) {
                    m_poll_entries[i] = m_entries[i];
                    m_poll_fds[i].events = m_entries[i].event == event_type::read ? POLLRDNORM : POLLWRNORM;
                    m_poll_fds[i].fd = m_entries[i].socket->handle();
                }
            }
        }

        //poll
        int poll_result = ::poll(m_poll_fds.data(), numeric_cast<unsigned int>(m_poll_fds.size()), timeout_ms);

        //process events
        if (poll_result > 0) {
            for (size_t i = 0; i < m_poll_fds.size() && poll_result > 0; ++i) {
                if (m_poll_fds[i].revents) {
                    //set the flags
                    status_flags flags;
                    flags.error              = m_poll_fds[i].revents & POLLERR;
                    flags.connection_aborted = m_poll_fds[i].revents & POLLHUP;
                    flags.invalid_socket     = m_poll_fds[i].revents & POLLNVAL;

                    //invoke the callback
                    m_poll_entries[i].callback(*this, m_poll_entries[i].socket, m_poll_entries[i].event, flags);

                    //count one less socket to check
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
    void socket_poller::set_on_socket_entry_added_callback(const std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_entry_added = f;
    }


    //Sets the callback that is invoked when a socket entry is removed.
    void socket_poller::set_on_socket_entry_remmoved_callback(const std::function<void(const size_t entries_count, const socket_ptr& s, event_type e, const event_callback_type& cb)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_entry_removed = f;
    }


    //Sets the callback that is invoked when a socket is removed.
    void socket_poller::set_on_socket_removed_callback(const std::function<void(const size_t entries_count, const socket_ptr& s)>& f) {
        std::lock_guard lock(m_mutex);
        m_on_socket_removed = f;
    }


    //Stops the socket poller, if not stopped yet.
    void socket_poller::stop() {
        {
            std::lock_guard lock(m_mutex);
            
            if (m_stop) {
                return;
            }
            
            m_stop = true;
        }
        m_com_socket->send(std::vector<char>(), m_com_socket_address);
    }


    //sets the entries as changed
    void socket_poller::set_entries_changed() {
        m_entries_changed = true;
        m_com_socket->send(std::vector<char>(), m_com_socket_address);
    }


} //namespace netlib
