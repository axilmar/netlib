#ifndef NETLIB_TUPLE_HPP
#define NETLIB_TUPLE_HPP


#include <utility>
#include <tuple>


namespace netlib {


    namespace internals {


        //flatten const tuple
        template <size_t Index, class Tpl, class F, class... T> auto flatten_tuple(const Tpl& tpl, F&& func, const T&... members) {
            if constexpr (Index < std::tuple_size_v<Tpl>) {
                return flatten_tuple<Index + 1>(tpl, std::forward<F>(func), members..., std::get<Index>(tpl));
            }
            else {
                return func(members...);
            }
        }


        //flatten mutable tuple
        template <size_t Index, class Tpl, class F, class... T> auto flatten_tuple(Tpl& tpl, F&& func, T&... members) {
            if constexpr (Index < std::tuple_size_v<Tpl>) {
                return flatten_tuple<Index + 1>(tpl, std::forward<F>(func), members..., std::get<Index>(tpl));
            }
            else {
                return func(members...);
            }
        }


    } //namespace internals


    /**
     * Flattens a const tuple, and invokes the given function on its members.
     * @param tpl tuple.
     * @param func function; receives a list of const references.
     * @return what the function returns.
     */
    template <class Tpl, class F> auto flatten_tuple(const Tpl& tpl, F&& func) {
        return internals::flatten_tuple<0>(tpl, std::forward<F>(func));
    }


    /**
     * Flattens a tuple, and invokes the given function on its members.
     * @param tpl tuple.
     * @param func function; receives a list of references.
     * @return what the function returns.
     */
    template <class Tpl, class F> auto flatten_tuple(Tpl& tpl, F&& func) {
        return internals::flatten_tuple<0>(tpl, std::forward<F>(func));
    }


} //namespace netlib


#endif //NETLIB_TUPLE_HPP
