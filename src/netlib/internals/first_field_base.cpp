#include <utility>
#include "netlib/internals/first_field_base.hpp"
#include "top_field.hpp"


namespace netlib::internals {


    //sets the top field to this.
    first_field_base::first_field_base() {
        top_field = this;
    }


    //sets the top field to this.
    first_field_base::first_field_base(const first_field_base& src)
        : field_base(src)
    {
        top_field = this;
    }


    //sets the top field to this.
    first_field_base::first_field_base(first_field_base&& src)
        : field_base(std::move(src))
    {
        top_field = this;
    }


} //namespace netlib::internals
