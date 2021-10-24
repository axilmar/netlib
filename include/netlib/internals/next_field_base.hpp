#ifndef NETLIB_NEXT_INTERNALS_FIELD_BASE_HPP
#define NETLIB_NEXT_INTERNALS_FIELD_BASE_HPP


#include "../field_base.hpp"


namespace netlib::internals {


    //establishes the next field in a message.
    class next_field_base : public field_base {
    protected:
        //the constructor.
        next_field_base();

        //the copy constructor.
        next_field_base(const next_field_base& src);

        //the move constructor.
        next_field_base(next_field_base&& src);
    };


} //namespace netlib::internals


#endif //NETLIB_NEXT_INTERNALS_FIELD_BASE_HPP
