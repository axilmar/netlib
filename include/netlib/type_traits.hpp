#ifndef NETLIB_TYPE_TRAITS_HPP
#define NETLIB_TYPE_TRAITS_HPP


#include <type_traits>
#include <tuple>


namespace netlib {


    /**
     * Get argument type at specific index.
     * Prototype.
     */
    template <size_t I, class F> struct arg_n;


    /**
     * Get argument type for function. 
     */
    template <size_t I, class R, class...A> struct arg_n<I, R(*)(A...)> {
        /*
         * The requested type. 
         */
        using type = std::tuple_element_t<I, std::tuple<A...>>;
    };


    /**
     * Get argument type for const member function.
     */
    template <size_t I, class R, class F, class...A> struct arg_n<I, R(F::*)(A...) const> {
        /*
         * The requested type.
         */
        using type = std::tuple_element_t<I, std::tuple<A...>>;
    };


    /**
     * Get argument type for member function.
     */
    template <size_t I, class R, class F, class...A> struct arg_n<I, R(F::*)(A...)> {
        /*
         * The requested type.
         */
        using type = std::tuple_element_t<I, std::tuple<A...>>;
    };


    /**
     * Shortcut for arg_n<I, F>::type. 
     */
    template <size_t I, class F> using arg_n_t = typename arg_n<I, F>::type;


} //namespace netlib


#endif //NETLIB_TYPE_TRAITS_HPP
