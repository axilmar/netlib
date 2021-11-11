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
        //find the start index
        size_t start_index = 0;
        _skip(type, "class ", start_index);
        _skip(type, "struct ", start_index);
        _skip(type, "union ", start_index);
        _skip(type, "enum ", start_index);

        //find namespace separator position
        size_t namespace_separator_position = std::string::npos;
        size_t template_parenthesis_count = 0;
        for (size_t offset = type.size(); offset > 0 && namespace_separator_position == std::string::npos; --offset) {
            switch (type[offset]) {
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
            result.first = type.substr(start_index, namespace_separator_position - start_index);
            result.second = type.substr(namespace_separator_position + 2, type.size() - (namespace_separator_position + 2));
        }

        //else there wasn't a namespace; get the name of the type
        else {
            result.second = type.substr(start_index, type.size() - start_index);
        }

        return result;
    }


} //namespace netlib::internals
