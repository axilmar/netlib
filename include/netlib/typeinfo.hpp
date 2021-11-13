#ifndef NETLIB_TYPEINFO_HPP
#define NETLIB_TYPEINFO_HPP


#include <string>
#include <utility>


namespace netlib {


    /**
     * Demangles the name returned by 'typeinfo'; not all compilers return full names.
     */
    std::string demangle_typeinfo_name(const char* name);


    /**
     * splits the full typename of a type to namespace and name.
     */
    std::pair<std::string, std::string> split_typeinfo_name(const std::string& type);


    /**
     * Returns the name of the given type. 
     */
    template <class T> std::string get_typeinfo_name() {
        const auto p = split_typeinfo_name(demangle_typeinfo_name(typeid(T).name()));
        return p.first.empty() ? p.second : p.first + "::" + p.second;
    }


} //namespace netlib


#endif //NETLIB_TYPEINFO_HPP
