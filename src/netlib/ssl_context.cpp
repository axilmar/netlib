#include "ssl.hpp"
#include "netlib/ssl_context.hpp"


namespace netlib::ssl {


    //The constructor.
    context::context(const std::shared_ptr<ssl_ctx_st>& ctx) : m_ctx(ctx) {
    }


} //namespace netlib::ssl
