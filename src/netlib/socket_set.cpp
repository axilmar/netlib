#include "netlib/socket_set.hpp"
#ifdef _WIN32
#define FD_SETSIZE (netlib::socket_set::max_sockets)
#endif
#include "netlib/platform.hpp"
#include <stdexcept>


namespace netlib {


    //The default constructor.
    socket_set::socket_set() {
        FD_ZERO(m_data);
    }


    //Adds a socket.
    void socket_set::add(socket&& s) {
        auto [it, ok] = m_sockets.insert(std::move(s));

        if (!ok) {
            throw std::invalid_argument("Socket already part of the set.");
        }

        FD_SET(s.handle(), m_data);
    }


    //Removes a socket.
    void socket_set::remove(socket& s) {
        if (m_sockets.erase(s) == 0) {
            throw std::invalid_argument("Socket not part of the set.");
        }

        FD_CLR(s.handle(), m_data);
    }


} //namespace netlib
