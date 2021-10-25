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
    template <class T, class Base> class field<T, Base, std::enable_if_t<!std::is_class_v<T>>> : public Base {
    public:
        /**
         * The field's value.
         */
        T value{};

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
         * Returns a copy of the value.
         * @return a copy of the value.
         */
        std::any get_value() const final {
            return value;
        }
    };


    /**
     * Field implementation for classes.
     * @param T type of class to inherit from.
     */
    template <class T, class Base> class field<T, Base, std::enable_if_t<std::is_class_v<T>>> : public Base, public T {
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
         * Returns a copy of the class.
         * @return a copy of the class.
         */
        std::any get_value() const final {
            return *this;
        }
    };


} //namespace netlib


#endif //NETLIB_FIELD_HPP
