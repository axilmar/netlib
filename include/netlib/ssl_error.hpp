#ifndef NETLIB_SSL_ERROR_HPP
#define NETLIB_SSL_ERROR_HPP


#include <stdexcept>


namespace netlib::ssl {


    /**
     * SSL error.
     */
    class error : public std::runtime_error {
    public:
        /**
         * Constructor.
         * The error message is retrieved from OpenSSL.
         */
        error(unsigned long code);

        /**
         * Constructor from string.
         * The error code is set to 0.
         * @param str string.
         */
        error(const std::string& str);

        /**
         * Returns the error code.
         */
        unsigned long code() const {
            return m_code;
        }

    private:
        unsigned long m_code;
    };


} //namespace netlib:ssl


#endif //NETLIB_SSL_ERROR_HPP
