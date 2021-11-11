#ifndef NETLIB_INTERNALS_TYPEINFO_HPP
#define NETLIB_INTERNALS_TYPEINFO_HPP


#include <string>
#include <utility>


namespace netlib::internals {


    //demangles the name returned by 'typeinfo'; not all compilers return full names.
    std::string demangle_typeinfo_name(const char* name);


    //splits the full typename of a type to namespace and name
    std::pair<std::string, std::string> split_typeinfo_name(const std::string& type);


} //namespace netlib::internals


#endif //NETLIB_INTERNALS_TYPEINFO_HPP
