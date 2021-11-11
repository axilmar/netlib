#include "netlib/internals/typeinfo.hpp"


#ifdef __GNUG__


#include <cstdlib>
#include <memory>
#include <cxxabi.h>


namespace netlib::internals {


    std::string demangle_typeinfo_name(const char* name) {
        int status = 0;
        std::unique_ptr<char, void(*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
        return (status == 0) ? res.get() : name;
    }


}


#else


namespace netlib::internals {


    std::string demangle_typeinfo_name(const char* name) {
        return name;
    }


}

#endif


namespace netlib::internals {


    //skip text
    static void _skip(const std::string& str, const std::string& txt, size_t& index) {
        for (size_t str_index = index, txt_index = 0; ; ++str_index, ++txt_index) {
            if (str_index == str.size()) {
                return;
            }

            if (txt_index == txt.size()) {
                index = str_index;
                return;
            }

            if (str[str_index] != txt[txt_index]) {
                return;
            }
        }
    }


    //split typeinfo name
    std::pair<std::string, std::string> split_typeinfo_name(const std::string& type) {
        size_t last_namespace_separator_index = -1;

        //skip possible type at the beginning
        size_t char_index = 0;
        _skip(type, "class ", char_index);
        _skip(type, "struct ", char_index);
        _skip(type, "union ", char_index);
        _skip(type, "enum ", char_index);

        //keep the start index
        const size_t start_index = char_index;

        //find namespace separators
        for (; char_index < type.size(); ++char_index) {

            //found namespace separator index; 
            if (type[char_index] == ':' && char_index < type.size() - 1 && type[char_index + 1] == ':') {
                last_namespace_separator_index = char_index;
                char_index += 2;
            }

            //found template arguments start; stop
            else if (type[char_index] == '<') {
                break;
            }
        }

        std::pair<std::string, std::string> result;

        //if there was a namespace, get it, and then get the name of the type
        if (last_namespace_separator_index != -1) {
            result.first = type.substr(start_index, last_namespace_separator_index - start_index);
            result.second = type.substr(last_namespace_separator_index + 2, char_index - (last_namespace_separator_index + 2));
        }

        //else there wasn't a namespace; get the name of the type
        else {
            result.second = type.substr(start_index, char_index - start_index);
        }

        return result;
    }


} //namespace netlib::internals
