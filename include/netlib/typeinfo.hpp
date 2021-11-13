#ifndef NETLIB_TYPEINFO_HPP
#define NETLIB_TYPEINFO_HPP


#include <string>
#include <utility>
#include <typeinfo>


namespace netlib {


    /**
     * Demangles the name returned by 'typeinfo'; not all compilers return full names.
     * @param name name as returned by typeinfo::name.
     * @return demangled name.
     */
    std::string demangle_typeinfo_name(const char* name);


    /**
     * splits the full typename of a type to namespace and name.
     * @param name the name of the type, demangled.
     * @return a pair of namespace and name.
     */
    std::pair<std::string, std::string> split_typeinfo_name(const std::string& name);


    /**
     * splits the full typename of a type to namespace and name.
     * @param name the name of the type as returned by typeinfo::name().
     * @return a pair of namespace and name.
     */
    inline std::pair<std::string, std::string> split_typeinfo_name(const char* name) {
        return split_typeinfo_name(demangle_typeinfo_name(name));
    }


    /**
     * splits the full typename of a type to namespace and name.
     * @param ti typeinfo of type.
     * @return a pair of namespace and name.
     */
    inline std::pair<std::string, std::string> split_typeinfo_name(const std::type_info& ti) {
        return split_typeinfo_name(ti.name());
    }


    /**
     * splits the full typename of a type to namespace and name.
     * @return a pair of namespace and name.
     */
    template <class T> std::pair<std::string, std::string> split_typeinfo_name() {
        return split_typeinfo_name(typeid(T));
    }


    /**
     * splits the full typename of a type to namespace and name.
     * @param obj object to get the name of.
     * @return a pair of namespace and name.
     */
    template <class T> std::pair<std::string, std::string> split_typeinfo_name(const T& obj) {
        return split_typeinfo_name(typeid(obj));
    }


    /**
     * Returns the name from the given typeinfo.
     * @param name name as returned by typeinfo::name.
     * @return the name of the type.
     */
    std::string get_typeinfo_name(const char* name);


    /**
     * Returns the name from the given typeinfo.
     * @param ti typeinfo.
     * @return the name of the type.
     */
    inline std::string get_typeinfo_name(const std::type_info& ti) {
        return get_typeinfo_name(ti.name());
    }


    /**
     * Returns the name of the given type.
     * @return the name of the type.
     */
    template <class T> std::string get_typeinfo_name() {
        return get_typeinfo_name(typeid(T));
    }


    /**
     * Returns the name of the given type.
     * @param obj object to get the name of.
     * @return the name of the type.
     */
    template <class T> std::string get_typeinfo_name(const T& obj) {
        return get_typeinfo_name(typeid(obj));
    }


} //namespace netlib


#endif //NETLIB_TYPEINFO_HPP
