#include "netlib/typeinfo.hpp"


#ifdef __GNUG__


#include <cstdlib>
#include <memory>
#include <cxxabi.h>


namespace netlib {


    std::string demangle_typeinfo_name(const char* name) {
        int status = 0;
        std::unique_ptr<char, void(*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
        return (status == 0) ? res.get() : name;
    }


}


#else


namespace netlib {


    std::string demangle_typeinfo_name(const char* name) {
        return name;
    }


}

#endif


namespace netlib {


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


    //skips the 'intro' of a type
    static size_t _skip_type_intro(const std::string& name) {
        size_t start_index = 0;
        _skip(name, "class ", start_index);
        _skip(name, "struct ", start_index);
        _skip(name, "union ", start_index);
        _skip(name, "enum ", start_index);
        return start_index;
    }


    //split typeinfo name to namespace and name.
    std::pair<std::string, std::string> split_typeinfo_name(const std::string& name) {
        //find the start index
        const size_t start_index = _skip_type_intro(name);

        //find namespace separator position
        size_t namespace_separator_position = std::string::npos;
        size_t template_parenthesis_count = 0;
        for (size_t offset = name.size(); offset > 0 && namespace_separator_position == std::string::npos; --offset) {
            switch (name[offset]) {
                case ':':
                    if (template_parenthesis_count == 0) {
                        namespace_separator_position = offset - 1;
                    }
                    else {
                        --offset;
                    }
                    break;

                case '>':
                    ++template_parenthesis_count;
                    break;

                case '<':
                    --template_parenthesis_count;
                    break;
            }
        }

        std::pair<std::string, std::string> result;

        //if there was a namespace, get it, and then get the name of the type
        if (namespace_separator_position != std::string::npos) {
            result.first = name.substr(start_index, namespace_separator_position - start_index);
            result.second = name.substr(namespace_separator_position + 2, name.size() - (namespace_separator_position + 2));
        }

        //else there wasn't a namespace; get the name of the type
        else {
            result.second = name.substr(start_index, name.size() - start_index);
        }

        return result;
    }


    //Returns the name from the given typeinfo.
    std::string get_typeinfo_name(const char* name) {
        const std::string dm = demangle_typeinfo_name(name);
        const size_t start_index = _skip_type_intro(dm);
        return dm.substr(start_index);
    }


} //namespace netlib
