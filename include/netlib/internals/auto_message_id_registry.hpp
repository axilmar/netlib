#ifndef NETLIB_INTERNALS_AUTO_MESSAGE_ID_TABLE_HPP
#define NETLIB_INTERNALS_AUTO_MESSAGE_ID_TABLE_HPP


#include "../message_id.hpp"


namespace netlib {


    template <class T, class Ref> class auto_message_id;

    
    namespace internals {


        class auto_message_id_registry {
        private:
            static void add(const char* entry);
            static message_id get(const char* entry);
            template <class T, class Ref> friend class auto_message_id;
        };


    } //namespace internals


} //namespace netlib


#endif //NETLIB_INTERNALS_AUTO_MESSAGE_ID_TABLE_HPP
