#ifndef NETLIB_SSL_SOCKET_HPP
#define NETLIB_SSL_SOCKET_HPP


#include <memory>
#include "socket.hpp"


struct ssl_ctx_st;
struct ssl_st;


namespace netlib::ssl {


    /**
     * Base class for sll sockets.
     */
    class socket : public netlib::socket {
    public:
        /**
         * Closes the socket. 
         */
        ~socket();

        /**
         * Returns the handle.
         */
        handle_type handle() const override;

        /**
         * Returns the pointer to the SSL context.
         */
        const std::shared_ptr<ssl_ctx_st>& ctx() const {
            return m_ctx;
        }

        /**
         * Returns the connection pointer.
         */
        const std::shared_ptr<ssl_st>& ssl() const {
            return m_ssl;
        }

    protected:
        /**
         * Constructor.
         * @param ctx context pointer.
         * @param ssl connection pointer.
         */
        socket(const std::shared_ptr<ssl_ctx_st>& ctx = nullptr, const std::shared_ptr<ssl_st>& ssl = nullptr) : m_ctx(ctx), m_ssl(ssl) {
        }

    private:
        std::shared_ptr<ssl_ctx_st> m_ctx;
        std::shared_ptr<ssl_st> m_ssl;
    };


} //namespace netlib::ssl


#endif //NETLIB_SSL_SOCKET_HPP
