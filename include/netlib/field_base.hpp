#ifndef NETLIB_FIELD_BASE_HPP
#define NETLIB_FIELD_BASE_HPP


#include <any>
#include "byte_buffer.hpp"


namespace netlib {


    namespace internals {
        class next_field_base;
    };


    /**
     * Base class for fields.
     */
    class field_base {
    public:
        /**
         * The destructor.
         */
        virtual ~field_base() {}

        /**
         * Returns the next field of the message.
         * @return the next field of the message.
         */
        field_base* get_next_field() const { return m_next_field; }

        /**
         * Interface for returning the type info of the underlying field's value.
         * @return the type info of the underlying field's value.
         */
        virtual const type_info& get_type_info() const = 0;

        /**
         * Interface for serializing this field.
         * @param buffer destination buffer.
         */
        virtual void serialize_this(byte_buffer& buffer) const = 0;

        /**
         * Interface for deserializing this field.
         * @param buffer deserialization buffer.
         * @param pos buffer index; on return, the next available position.
         */
        virtual void deserialize_this(const byte_buffer& buffer, byte_buffer::position& pos) = 0;

        /**
         * Returns a const reference of the value.
         * @return a const reference of the value.
         */
        virtual std::any get_value() const = 0;

        /**
         * Returns a reference of the value.
         * @return a reference of the value.
         */
        virtual std::any get_value() = 0;

    protected:
        /**
         * The default constructor.
         */
        field_base() : m_next_field(nullptr) {}

        /**
         * The copy constructor.
         * @param src the source field.
         */
        field_base(const field_base& src) : m_next_field(nullptr) {}

        /**
         * The move constructor.
         * @param src the source field.
         */
        field_base(field_base&& src) : m_next_field(nullptr) {}

        /**
         * The copy assignment operator.
         * @param src the source field.
         * @return reference to this.
         */
        field_base& operator = (const field_base& src) { return *this; }

        /**
         * The move assignment operator.
         * @param src the source field.
         * @return reference to this.
         */
        field_base& operator = (field_base&& src) { return *this; }

    private:
        //next field
        field_base* m_next_field;

        //accesses the next field.
        friend class internals::next_field_base;
    };


} //namespace netlib


#endif //NETLIB_FIELD_BASE_HPP
