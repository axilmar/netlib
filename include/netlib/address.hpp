#ifndef NETLIB_ADDRESS_HPP
#define NETLIB_ADDRESS_HPP


namespace netlib {


    /**
     * Base class for addresses. 
     */
    class address {
    public:
        /**
         * Virtual destructor due to subclassing. 
         */
        virtual ~address() {}
    };


} //namespace netlib


#endif //NETLIB_ADDRESS_HPP
