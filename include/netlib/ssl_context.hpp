#ifndef NETLIB_SSL_CONTEXT_HPP
#define NETLIB_SSL_CONTEXT_HPP


#include <memory>


struct ssl_ctx_st;


namespace netlib::ssl {


    /**
     * Base class for SSL contexts.
     */
    class context {
    public:
        /**
         * The destructor.
         * Virtual due to inheritance.
         */
        virtual ~context() {
        }

        /**
         * Returns the pointer to the SSL context.
         */
        const std::shared_ptr<ssl_ctx_st>& ctx() const {
            return m_ctx;
        }

    protected:
        /**
         * The constructor.
         * @param ctx context.
         */
        context(const std::shared_ptr<ssl_ctx_st>& ctx);

    private:
        std::shared_ptr<ssl_ctx_st> m_ctx;
    };


} //namespace netlib::ssl


#endif //NETLIB_SSL_CONTEXT_HPP
