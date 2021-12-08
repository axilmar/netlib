#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include "socket_handle.hpp"
#include "byte_buffer.hpp"


namespace netlib {


    /**
     * Base class for sockets. 
     */
    class socket {
    public:
        /**
         * The destructor.
         * It closes the socket.
         */
        ~socket();

        /**
         * Returns the socket handle.
         * @return the socket handle.
         */
        socket_handle handle() const { return m_handle; }

        /**
         * Checks if this socket is valid or invalid.
         * @return true if valid, false if invalid.
         */
        operator bool() const;

        /**
         * Compares this object with the given one.
         * @param other the other object to compare this to.
         * @return -1 if this is less than the given object, 0 if they are equal, 1 if this is greater than the given object.
         */
        int compare(const socket& other) const {
            return m_handle < other.m_handle ? -1 : m_handle > other.m_handle ? 1 : 0;
        }

        /**
         * Checks if this and the given object are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const socket& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if this and the given object are diferent.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const socket& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object comes before the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const socket& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object comes after the given object.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const socket& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object comes before the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const socket& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object comes after the given object or if they are equal.
         * @param other the other object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const socket& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hashcode of this address.
         */
        size_t hash() const {
            return std::hash<socket_handle>()(m_handle);
        }

    protected:
        /**
         * The default constructor.
         * The socket handle is initialized to an invalid socket value.
         */
        socket();

        /**
         * Constructor from handle. 
         * @param handle the socket's handle. 
         */
        socket(socket_handle handle);

        /**
         * Sockets are not copyable. 
         */
        socket(const socket&) = delete;

        /**
         * The move constructor.
         * @param src source object.
         */
        socket(socket&& src);

        /**
         * Sockets are not copyable.
         */
        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator.
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

    private:
        //handle
        socket_handle m_handle;

        //closes the socket.
        void close();
    };


} //namespace netlib


namespace std {
    template <> struct hash<netlib::socket> {
        size_t operator ()(const netlib::socket& s) const {
            return s.hash();
        }
    };
}


#endif //NETLIB_SOCKET_HPP
