#ifndef NETLIB_STRINGSTREAM_HPP
#define NETLIB_STRINGSTREAM_HPP


#include <string>
#include <sstream>


namespace netlib {


    /**
     * Helper stringstream class, based on std::stringstream.
     * It provides an operator std::string(), so as that it is possible to write stringstream() << value1 << value2 << etc
     * and pass that expression directly to where a string is needed.
     */
    class stringstream : public std::stringstream {
    public:
        using std::stringstream::stringstream;

        using std::stringstream::operator =;

        /**
         * Automatically converts the string to a stream.
         * @return a string.
         */
        operator std::string() const {
            return std::stringstream::str();
        }
    };


} //namespace netlib


#endif //NETLIB_STRINGSTREAM_HPP
