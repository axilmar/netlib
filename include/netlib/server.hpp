#ifndef NETLIB_SERVER_HPP
#define NETLIB_SERVER_HPP


#include <memory>
#include "socket.hpp"


namespace netlib {


    /**
     * Base class for servers.
     */
    class server : public std::enable_shared_from_this<server> {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~server() {}

        /**
         * The copy assignment operator.
         * Server objects are not copyable, due to their complexity.
         */
        server& operator = (const server&) = delete;

        /**
         * The move assignment operator.
         * Server objects are not movable, due to their complexity.
         */
        server& operator = (server&&) = delete;

    protected:
        /**
         * The default constructor.
         */
        server() {}

        /**
         * The copy constructor.
         * Server objects are not copyable, due to their complexity.
         */
        server(const server&) = delete;

        /**
         * The move constructor.
         * Server objects are not movable, due to their complexity.
         */
        server(server&&) = delete;

        /**
         * The open connection event.
         * @param s socket that was used or created for the new client.
         * @param addr address of client that wants to connect.
         * @return true if the connection is accepted, false otherwise.
         */
        virtual bool on_new_connection(socket& s, const socket_address& addr) = 0;

        /**
         * The close connection event.
         * Invoked when a socket is detected as closed.
         * @param s socket that was closed.
         * @param addr address of client that was closed.
         */
        virtual void on_connection_closed(socket& s, const socket_address& addr) = 0;

        /**
         * The receive event.
         * @param s socket to read data from.
         * @return true if data were read successfully, false if the socket was closed.
         */
        virtual bool on_receive(socket& s) = 0;
    };


} //namespace netlib


#endif //NETLIB_SERVER_HPP
