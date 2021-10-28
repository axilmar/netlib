#ifndef NETLIB_SERIALIZATION_TRAITS_HPP
#define NETLIB_SERIALIZATION_TRAITS_HPP


#include <type_traits>
#include <utility>
#include <tuple>
#include <memory>


namespace netlib {


    /**
     * Trait for recognizing if T is container;
     * it checks if T provides begin() const and end() const functions which returns const_iterators.
     */
    template <class T> struct has_begin_end {
    private:
        struct true_type { char c[1]; };
        struct false_type { char c[2]; };
        template <class C> static auto get_func(typename C::const_iterator (C::*m)() const) { return m; };
        template <class C> static true_type test_begin(decltype(get_func<C>(&C::begin)));
        template <class C> static false_type test_begin(...);
        template <class C> static true_type test_end(decltype(get_func<C>(&C::end)));
        template <class C> static false_type test_end(...);

    public:
        /**
         * True if T is container, false otherwise.
         */
        static constexpr bool value = sizeof(test_begin<T>(nullptr)) == sizeof(true_type) && sizeof(test_end<T>(nullptr)) == sizeof(true_type);
    };


    /**
     * Variable for has_begin_end<T> template.
     */
    template <class T> inline constexpr bool has_begin_end_v = has_begin_end<T>::value;


    /**
     * Trait for recognizing if T is container that its storage is an array;
     * it checks if T provides the data() function.
     */
    template <class T> struct has_data_array {
    private:
        struct true_type { char c[1]; };
        struct false_type { char c[2]; };
        template <class C> static auto get_func(typename const typename T::value_type* (C::* m)() const) { return m; };
        template <class C> static true_type test(decltype(get_func<C>(&C::data)));
        template <class C> static false_type test(...);

    public:
        /**
         * True if T is container with data() function, false otherwise.
         */
        static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(true_type);
    };


    /**
     * Variable for has_data_array<T> template.
     */
    template <class T> inline constexpr bool has_data_array_v = has_data_array<T>::value;


    /**
     * Trait for recognizing if T is sequence container;
     * it checks if T provides push_back function.
     */
    template <class T> struct has_push_back {
    private:
        struct true_type { char c[1]; };
        struct false_type { char c[2]; };
        template <class C> static auto get_copy_func(void (C::* m)(const typename C::value_type&)) { return m; };
        template <class C> static auto get_copy_func(void (C::* m)(typename C::value_type)) { return m; };
        template <class C> static auto get_move_func(void (C::* m)(typename C::value_type&&)) { return m; };
        template <class C> static true_type test_push_back_copy(decltype(get_copy_func<C>(&C::push_back)));
        template <class C> static false_type test_push_back_copy(...);
        template <class C> static true_type test_push_back_move(decltype(get_move_func<C>(&C::push_back)));
        template <class C> static false_type test_push_back_move(...);

    public:
        /**
         * True if T is sequence container, false otherwise.
         */
        static constexpr bool value = sizeof(test_push_back_copy<T>(nullptr)) == sizeof(true_type) || sizeof(test_push_back_move<T>(nullptr)) == sizeof(true_type);
    };


    /**
     * Variable for has_push_back<T> template.
     */
    template <class T> inline constexpr bool has_push_back_v = has_push_back<T>::value;


    /**
     * Trait for recognizing if T is associative container;
     * it checks if T provides insert function.
     */
    template <class T> struct has_insert {
    private:
        struct true_type { char c[1]; };
        struct false_type { char c[2]; };
        template <class C> static auto get_copy_func(std::pair<typename C::iterator, bool> (C::* m)(const typename C::value_type&)) { return m; };
        template <class C> static auto get_move_func(std::pair<typename C::iterator, bool> (C::* m)(typename C::value_type&&)) { return m; };
        template <class C> static true_type test_insert_copy(decltype(get_copy_func<C>(&C::insert)));
        template <class C> static false_type test_insert_copy(...);
        template <class C> static true_type test_insert_move(decltype(get_move_func<C>(&C::insert)));
        template <class C> static false_type test_insert_move(...);

    public:
        /**
         * True if T is associative container, false otherwise.
         */
        static constexpr bool value = sizeof(test_insert_copy<T>(nullptr)) == sizeof(true_type) || sizeof(test_insert_move<T>(nullptr)) == sizeof(true_type);
    };


    /**
     * Variable for has_insert<T> template.
     */
    template <class T> inline constexpr bool has_insert_v = has_insert<T>::value;


    /**
     * Trait for tuple types.
     */
    template <class T> struct is_tuple : std::false_type {
    };


    /**
     * Specialization for pair.
     */
    template <class T1, class T2> struct is_tuple<std::pair<T1, T2>> : std::true_type {
    };


    /**
     * Specialization for tuple.
     */
    template <class... T> struct is_tuple<std::tuple<T...>> : std::true_type {
    };


    /**
     * Variable for is_tuple<T> template.
     */
    template <class T> inline constexpr bool is_tuple_v = is_tuple<T>::value;


    /**
     * Trait for pointers.
     */
    template <class T> struct is_pointer : std::false_type {
    };


    /**
     * Specialization for T*.
     */
    template <class T> struct is_pointer<T*> : std::true_type {
    };


    /**
     * Specialization for std::unique_ptr<T, Deleter>.
     */
    template <class T, class Deleter> struct is_pointer<std::unique_ptr<T, Deleter>> : std::true_type {
    };


    /**
     * Specialization for std::shared_ptr<T>.
     */
    template <class T> struct is_pointer<std::shared_ptr<T>> : std::true_type {
    };


    /**
     * Variable for is_pointer<T> template.
     */
    template <class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;


    /**
     * Create object for raw pointer.
     * @param ptr ptr to set.
     * @return reference to object that was created.
     */
    template <class T> T& make_object(T*& ptr) {
        ptr = new T();
        return *ptr;
    }


    /**
     * Create object for unique pointer.
     * @param ptr ptr to set.
     * @return reference to object that was created.
     */
    template <class T, class Deleter> T& make_object(std::unique_ptr<T, Deleter>& ptr) {
        ptr = std::make_unique<T>();
        return *ptr.get();
    }


    /**
     * Create object for shared pointer.
     * @param ptr ptr to set.
     * @return reference to object that was created.
     */
    template <class T> T& make_object(std::shared_ptr<T>& ptr) {
        ptr = std::make_shared<T>();
        return *ptr.get();
    }


} //namespace netlib


#endif //NETLIB_SERIALIZATION_TRAITS_HPP
