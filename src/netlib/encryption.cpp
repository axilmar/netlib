#include <random>
#include "netlib/encryption.hpp"


namespace netlib {


    //Creates a random key, useful for the xor encryption algorithm.
    byte_buffer create_random_key(const size_t size) {
        std::random_device r;
        std::default_random_engine e(r());

        std::uniform_int_distribution<int> d(0, static_cast<int>(std::numeric_limits<byte_buffer::value_type>::max()));
        
        byte_buffer result(size);
        
        for (byte_buffer::value_type& v : result) {
            v = static_cast<byte_buffer::value_type>(d(e));
        }
        
        return result;
    }


} //namespace netlib
