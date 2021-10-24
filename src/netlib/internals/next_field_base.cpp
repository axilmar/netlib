#include <utility>
#include "netlib/internals/next_field_base.hpp"
#include "top_field.hpp"


namespace netlib::internals {


//avoid code repetition, avoid member functions
#define SET_NEXT_FIELD()\
    top_field->m_next_field = this;\
    top_field = this


    //sets this as the next field of the previous field; top must have been set.
    next_field_base::next_field_base() {
        SET_NEXT_FIELD();
    }


    //sets this as the next field of the previous field; top must have been set.
    next_field_base::next_field_base(const next_field_base& src)
        : field_base(src)
    {
        SET_NEXT_FIELD();
    }


    //sets this as the next field of the previous field; top must have been set.
    next_field_base::next_field_base(next_field_base&& src)
        : field_base(std::move(src))
    {
        SET_NEXT_FIELD();
    }


} //namespace netlib::internals
