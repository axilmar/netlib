#ifndef NETLIB_FIELD_HPP
#define NETLIB_FIELD_HPP


#include "serialization.hpp"
#include "internals/next_field_base.hpp"


namespace netlib {


    //field prototype.
    template <class T, class Base = internals::next_field_base, class Enable = void> class field;


    /**
     * Declaration of a non-class field.
     * @param T type of field.
     */
    template <class T, class Base> class field<T, Base, std::enable_if_t<!std::is_class_v<T> || is_pointer_v<T>>> : public Base {
    public:
        /**
         * The field's value.
         */
        T value;

        /**
         * The default constructor.
         */
        field() : value{} {}

        /**
         * Constructor from value.
         */
        template <class Y> field(Y&& v) : value(std::forward<Y>(v)) {}

        /**
         * Used as const ref.
         */
        operator const T& () const {
            return value;
        }

        /**
         * Used as ref.
         */
        operator T& () {
            return value;
        }

        /**
         * Assigment from value.
         * @param v source value.
         * @return reference to this.
         */
        template <class V> field& operator = (V&& v) {
            value = std::forward<V>(v);
            return *this;
        }

        /**
         * Implementation of returning the typeinfo of the field.
         * @return the type info of the field.
         */
        const type_info& get_type_info() const final {
            return typeid(T);
        }

        /**
         * Implementation of the field's serialization.
         * It invokes the standalone version of the function.
         * @param buffer output buffer.
         */
        void serialize_this(byte_buffer& buffer) const final {
            serialize(value, buffer);
        }

        /**
         * Implementation of the field's deserialization.
         * It invokes the standalone version of the function.
         * @param buffer input buffer.
         * @param pos current position.
         */
        void deserialize_this(const byte_buffer& buffer, byte_buffer::position& pos) final {
            deserialize(value, buffer, pos);
        }

        /**
         * Returns a const reference of the value.
         * @return a const reference of the value.
         */
        std::any get_value() const final {
            return std::cref(value);
        }

        /**
         * Returns a reference of the value.
         * @return a reference of the value.
         */
        std::any get_value() final {
            return std::ref(value);
        }

        /**
         * If T is a pointer type, then return pointer value.
         */
        const T& operator ->() const {
            return value;
        }
    };


    /**
     * Field implementation for classes.
     * @param T type of class to inherit from; it must not be a pointer class.
     */
    template <class T, class Base> class field<T, Base, std::enable_if_t<std::is_class_v<T> && !is_pointer_v<T>>> : public Base, public T {
    public:
        using T::T;

        /**
         * Implementation of returning the typeinfo of the field.
         * @return the type info of the field.
         */
        const type_info& get_type_info() const final {
            return typeid(T);
        }

        /**
         * Implementation of the field's serialization.
         * It invokes the standalone version of the function.
         * @param buffer output buffer.
         */
        void serialize_this(byte_buffer& buffer) const final {
            ::serialize(static_cast<const T&>(*this), buffer);
        }

        /**
         * Implementation of the field's deserialization.
         * It invokes the standalone version of the function.
         * @param buffer input buffer.
         * @param pos current position.
         */
        void deserialize_this(const byte_buffer& buffer, byte_buffer::position& pos) final {
            ::deserialize(static_cast<T&>(*this), buffer, pos);
        }

        /**
         * Returns a const reference of the underlying object.
         * @return a const reference of the underlying object.
         */
        std::any get_value() const final {
            return std::cref(*this);
        }

        /**
         * Returns a reference of the underlying object.
         * @return a reference of the underlying object.
         */
        std::any get_value() final {
            return std::ref(*this);
        }
    };


} //namespace netlib


#endif //NETLIB_FIELD_HPP
