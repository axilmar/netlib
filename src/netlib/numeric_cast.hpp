#ifndef NETLIB_NUMERIC_CAST_HPP
#define NETLIB_NUMERIC_CAST_HPP


#include <type_traits>
#include <typeinfo>
#include <limits>
#include <string>


namespace netlib {


    /**
     * Bad narrow cast exception. 
     */
    class bad_narrow_cast : public std::bad_cast {
    public:
        /**
         * Constructor.
         * @param msg message.
         */
        bad_narrow_cast(std::string&& msg) : std::bad_cast(), m_message(std::move(msg)) {
        }

        /**
         * Returns the message.
         * @return the message.
         */
        const char* what() const override {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };


    /**
     * Widening conversion. 
     * @param src source value; must be smaller than or equal to the destination value.
     * @return the destination value.
     */
    template <class Dst, class Src> std::enable_if_t < sizeof(Dst) >= sizeof(Src), Dst > numeric_cast(Src src) {
        return static_cast<Dst>(src);
    }


    /**
     * Narrowing conversion.
     * @param src source value; must be wider than the destination value.
     * @return the destination value.
     * @exception bad_narrow_cast thrown if the source value is greater than the max value of the destination value.
     */
    template <class Dst, class Src> std::enable_if_t<sizeof(Dst) < sizeof(Src), Dst> numeric_cast(Src src) {
        if (src > std::numeric_limits<Dst>::max()) {
            throw bad_narrow_cast("Source value overflow in narrowing cast.");
        }
        return static_cast<Dst>(src);
    }


} //namespace netlib


#endif //NETLIB_NUMERIC_CAST_HPP
