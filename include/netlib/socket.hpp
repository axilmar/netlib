#ifndef NETLIB_SOCKET_HPP
#define NETLIB_SOCKET_HPP


#include <cstdint>
#include <atomic>
#include <functional>
#include "socket_address.hpp"


namespace netlib {


    /**
     * Base class for sockets.
     */
    class socket {
    public:
        /**
         * handle type.
         */
        using handle_type = uintptr_t;

        /**
         * Invalid handle.
         */
        static constexpr handle_type invalid_handle = ~handle_type(0);

        /**
         * The default constructor.
         * @param handle optional socket handle.
         */
        socket(handle_type handle = invalid_handle);

        /**
         * The copy constructor. 
         * Sockets are not copyable.
         */
        socket(const socket&) = delete;

        /**
         * The move constructor. 
         * @param src source object.
         */
        socket(socket&& src);

        /**
         * The destructor.
         * closes the socket if its reference count drops to 0.
         */
        virtual ~socket();

        /**
         * The copy assignment operator. 
         * Sockets are not copyable.
         */
        socket& operator = (const socket&) = delete;

        /**
         * The move assignment operator. 
         * @param src source object.
         * @return reference to this.
         */
        socket& operator = (socket&& src);

        /**
         * Returns the handle.
         */
        handle_type handle() const;

        /**
         * Returns true if the socket is valid, false otherwise.
         */
        explicit operator bool() const;

        /**
         * Returns the address this socket is bound to.
         */
        socket_address bound_address() const;

        /**
         * Compares the socket handles.
         * @param other the other object to compare this to.
         * @return less than zero if this comes before the given object, 
         *  greater than zero if this comes after the given object,
         *  0 if the handles are equal.
         */
        int compare(const socket& other) const;

        /**
         * Checks if the two objects are equal.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator == (const socket& other) const {
            return compare(other) == 0;
        }

        /**
         * Checks if the two objects are different.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator != (const socket& other) const {
            return compare(other) != 0;
        }

        /**
         * Checks if this object is less than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator < (const socket& other) const {
            return compare(other) < 0;
        }

        /**
         * Checks if this object is greater than the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator > (const socket& other) const {
            return compare(other) > 0;
        }

        /**
         * Checks if this object is less than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator <= (const socket& other) const {
            return compare(other) <= 0;
        }

        /**
         * Checks if this object is greater than or equal to the given one.
         * @return other the object to compare this to.
         * @return true on success, false on failure.
         */
        bool operator >= (const socket& other) const {
            return compare(other) >= 0;
        }

        /**
         * Returns the hash code for this object.
         */
        size_t hash() const;

    private:
        //handle
        handle_type m_handle;
    };


} //namespace netlib


namespace std {


    /**
     * Specialization of std::hash for netlib::socket.
     */
    template <> struct hash<netlib::socket> {
        /**
         * Returns addr.hash().
         * @param addr object to get the hash of.
         * @return the object's hash.
         */
        size_t operator ()(const netlib::socket& addr) const {
            return addr.hash();
        }
    };


} //namespace std


#endif //NETLIB_SOCKET_HPP
