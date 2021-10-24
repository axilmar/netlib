#ifndef NETLIB_INTERNALS_TOP_FIELD_HPP
#define NETLIB_INTERNALS_TOP_FIELD_HPP


#include "netlib/field_base.hpp"


namespace netlib::internals {


    //top field declaration.
    extern thread_local field_base* top_field;


} //namespace netlib::internals


#endif //NETLIB_INTERNALS_TOP_FIELD_HPP
