#ifndef NETLIB_FIRST_INTERNALS_FIELD_BASE_HPP
#define NETLIB_FIRST_INTERNALS_FIELD_BASE_HPP


#include "../field_base.hpp"


namespace netlib::internals {


    //establishes the first field of a message.
    class first_field_base : public field_base {
    protected:
        //the default constructor.
        first_field_base();

        //the copy constructor.
        first_field_base(const first_field_base& src);

        //the move constructor.
        first_field_base(first_field_base&& src);
    };


} //namespace netlib::internals


#endif //NETLIB_FIRST_INTERNALS_FIELD_BASE_HPP
