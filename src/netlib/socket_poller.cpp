#include "platform.hpp"
#include "netlib/socket_poller.hpp"
#include "netlib/numeric_cast.hpp"


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


    //creates the com socket
    static socket::handle_type create_com_socket() {
        //create socket
        socket::handle_type s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        
        //handle error
        if (s < 0) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }
        
        //bound address
        socket_address addr(ip_address::ip4::loopback, 0);

        //bind
        if (::bind(s, reinterpret_cast<const sockaddr*>(addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //connect
        socket_address bound_addr = socket::bound_address(s);
        if (::connect(s, reinterpret_cast<const sockaddr*>(bound_addr.data()), sizeof(sockaddr_storage))) {
            throw std::system_error(get_last_error_number(), std::system_category());
        }

        //success
        return s;
    }


    //the constructor.
    socket_poller::socket_poller(size_t max_sockets)
        : m_max_sockets(max_sockets + 1) //account for the com socket
        , m_com_socket(create_com_socket())
        , m_entries(1) //account for the com socket
        , m_entries_changed{}
        , m_stop{}
        , m_poll_counter{0}
    {
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
            m_mutex.lock();

            //if stopped
            if (m_stop) {
                m_mutex.unlock();
                return poll_status::stopped;
            }

            //if there are no entries, wait for internal socket entry;
            //account for the com socket
            while (m_entries.size() - 1 == 0) {
                m_mutex.unlock();

                //wait for data
                char buf;
                int s = recv(m_com_socket, &buf, sizeof(buf), 0);

                //if there was an error
                if (s <= 0) {
                    if (is_socket_closed_error(get_last_error_number())) {
                        return poll_status::stopped;
                    }
                    throw std::system_error(get_last_error_number(), std::system_category());
                }

                //no error
                m_mutex.lock();
            }

            m_mutex.unlock();

            std::lock_guard lock(m_mutex);

            //if changed, rebuild the m_poll_entries/m_poll_fds arrays
            if (m_entries_changed) {
                m_entries_changed = false;

                //make room for new entries
                m_poll_entries.resize(m_entries.size());
                m_poll_fds.resize(m_entries.size());

                //set the internal entry
                m_poll_fds[0].events = POLLRDNORM;
                m_poll_fds[0].fd = m_com_socket;

                //set m_poll_entries
                for (size_t i = 1; i < m_entries.size(); ++i) {
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
            //process the internal com socket
            if (m_poll_fds[0].revents) {
                char buf;
                recv(m_com_socket, &buf, sizeof(buf), 0);
                --poll_result;
            }

            //process entries
            for (size_t i = 1; i < m_poll_fds.size() && poll_result > 0; ++i) {
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
        char buf = 0;
        ::send(m_com_socket, &buf, sizeof(buf), 0);
    }


    //sets the entries as changed
    void socket_poller::set_entries_changed() {
        m_entries_changed = true;
        char buf = 0;
        ::send(m_com_socket, &buf, sizeof(buf), 0);
    }


} //namespace netlib
