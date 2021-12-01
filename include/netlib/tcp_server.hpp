#ifndef NETLIB_TCP_SERVER_HPP
#define NETLIB_TCP_SERVER_HPP


#include "execlib/executor.hpp"
#include "server.hpp"


namespace netlib {


    /**
     * A server that uses the tcp/ip protocol for communicating with the clients.
     */
    class tcp_server : public server {
    public:
        /**
         * The constructor.
         * @param listen_addresses addresses to listen for connections.
         * @param executor executor to use for parallelizing reception and processing of incoming data.
         * @exception std::invalid_argument thrown if the list of listen addresses is empty or the executor pointer is null.
         */
        tcp_server(const std::vector<socket_address>& listen_addresses, const std::shared_ptr<execlib::executor>& executor = std::make_shared<execlib::executor>());

        /**
         * Stops the server.
         */
        ~tcp_server();

    protected:

    private:
    };


} //namespace netlib


#endif //NETLIB_TCP_SERVER_HPP
