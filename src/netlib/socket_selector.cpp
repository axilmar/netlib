#include "netlib/socket_selector.hpp"


namespace netlib {


    //internal process set function
    template <class F>
    static void _process_socket_set(socket_set& dst, const socket_set& src, F&& func) {
        for (const auto& entry : src.get_socket_map()) {
            func(dst, *entry.second);
        }
    }


    //Returns true if it contains the given socket for the given operation.
    bool socket_selector::contains_socket_for_reading(socket& s) const {
        std::lock_guard lock(m_mutex);
        return m_read_set.contains(s);
    }


    //Adds a socket for reading.
    void socket_selector::add_socket_for_reading(socket& s) {
        std::lock_guard lock(m_mutex);
        m_read_set.add(s);
    }


    //Removes a socket for reading.
    void socket_selector::remove_socket_for_reading(socket& s) {
        std::lock_guard lock(m_mutex);
        m_read_set.remove(s);
    }


    //Returns true if it contains the given socket for the given operation.
    bool socket_selector::contains_socket_for_writing(socket& s) const {
        std::lock_guard lock(m_mutex);
        return m_write_set.contains(s);
    }


    //Adds a socket for writing.
    void socket_selector::add_socket_for_writing(socket& s) {
        std::lock_guard lock(m_mutex);
        m_write_set.add(s);
    }


    //Removes a socket for writing.
    void socket_selector::remove_socket_for_writing(socket& s) {
        std::lock_guard lock(m_mutex);
        m_write_set.remove(s);
    }


    //Returns true if it contains the given socket for the given operation.
    bool socket_selector::contains_socket_for_error_checking(socket& s) const {
        std::lock_guard lock(m_mutex);
        return m_except_set.contains(s);
    }


    //Adds a socket for excepting.
    void socket_selector::add_socket_for_error_checking(socket& s) {
        std::lock_guard lock(m_mutex);
        m_except_set.add(s);
    }


    //Removes a socket for excepting.
    void socket_selector::remove_socket_for_error_checking(socket& s) {
        std::lock_guard lock(m_mutex);
        m_except_set.remove(s);
    }


    //Adds a socket set for reading.
    void socket_selector::add_socket_set_for_reading(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_read_set, set, [](socket_set& dst, socket& s) {
            dst.add(s);
        });
    }


    //Removes a socket set for reading.
    void socket_selector::remove_socket_set_for_reading(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_read_set, set, [](socket_set& dst, socket& s) {
            dst.remove(s);
        });
    }


    //Empties the socket set for reading.
    void socket_selector::clear_socket_set_for_reading() {
        std::lock_guard lock(m_mutex);
        m_read_set.clear();
    }


    //Adds a socket set for writing.
    void socket_selector::add_socket_set_for_writing(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_write_set, set, [](socket_set& dst, socket& s) {
            dst.add(s);
            });
    }


    //Removes a socket set for writing.
    void socket_selector::remove_socket_set_for_writing(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_write_set, set, [](socket_set& dst, socket& s) {
            dst.remove(s);
        });
    }


    //Empties the socket set for writing.
    void socket_selector::clear_socket_set_for_writing() {
        std::lock_guard lock(m_mutex);
        m_write_set.clear();
    }


    //Adds a socket set for error_checking.
    void socket_selector::add_socket_set_for_error_checking(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_except_set, set, [](socket_set& dst, socket& s) {
            dst.add(s);
        });
    }


    //Removes a socket set for error_checking.
    void socket_selector::remove_socket_set_for_error_checking(const socket_set& set) {
        std::lock_guard lock(m_mutex);
        _process_socket_set(m_except_set, set, [](socket_set& dst, socket& s) {
            dst.remove(s);
        });
    }


    //Empties the socket set for error_checking.
    void socket_selector::clear_socket_set_for_error_checking() {
        std::lock_guard lock(m_mutex);
        m_except_set.clear();
    }


} //namespace netlib
