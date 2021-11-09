#ifndef NETLIB_BYTE_HPP
#define NETLIB_BYTE_HPP


namespace netlib {


    /**
     * Byte type.
     * std::byte is not suitable for this, because:
     * a) arithmetic/shift operations are not available on it,
     * b) std::numeric_limits<std::byte>::max() returns 0 (at least on msvc).
     */
    using byte = unsigned char;


} //namespace netlib


#endif //NETLIB_BYTE_HPP