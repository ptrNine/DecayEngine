#ifndef DECAYENGINE_SERIALIZATION_HPP
#define DECAYENGINE_SERIALIZATION_HPP
#include "ftl/array.hpp"
#include "ftl/vector.hpp"
#include "defines.hpp"

// Todo: refactor array implementation (add serialization of array size to dynamic arrays)
#define SERIALIZE_METHOD_CONST_SIZE() \
    static constexpr auto c_serialize_size()

#define SERIALIZE_METHOD_SIZE() \
    auto serialize_size() const

#define SERIALIZE_METHOD() \
    inline auto serialize() const { return srlz::serialize(*this); } \
    void serialize_impl(Byte* _serializer, SizeT _serializer_pos = 0) const

#define SERIALIZE(VALUE) \
    srlz::_serialize_tmpl(_serializer + _serializer_pos, (VALUE)); \
    _serializer_pos += srlz::_sizeof_tmpl(VALUE)

#define SERIALIZE_ARRAY(PTR, SIZE) { \
    auto temp = srlz::_sizeof_array_tmpl(PTR, SIZE); \
    srlz::_serialize_array_tmpl(_serializer + _serializer_pos, PTR, SIZE, temp); \
    _serializer_pos += srlz::_sizeof_array_unpacker(temp, SIZE); \
}


#define DESERIALIZE_METHOD() \
    inline auto deserialize(const Byte* buf) { return srlz::deserialize(*this, buf); } \
    void deserialize_impl(const Byte* _deserializer, SizeT _deserializer_pos = 0)

#define DESERIALIZE(VALUE) \
    srlz::_deserialize_tmpl((VALUE), _deserializer + _deserializer_pos); \
    _deserializer_pos += srlz::_sizeof_tmpl(VALUE)

#define DESERIALIZE_ARRAY(PTR, SIZE) \
    _deserializer_pos += srlz::_deserialize_array_tmpl(PTR, SIZE,_deserializer + _deserializer_pos);



#define SERIALIZE_GET_SIZE_ARRAY(PTR, SIZE) \
    srlz::_sizeof_array_unpacker(srlz::_sizeof_array_tmpl((PTR), (SIZE)), SIZE)

#define SERIALIZE_GET_SIZE(VALUE) \
    srlz::_sizeof_tmpl(VALUE)

#define C_SERIALIZE_GET_SIZE(VALUE) \
    srlz::_const_sizeof_tmpl<decltype(VALUE)>()



#define IS_SERIALIZABLE_IMPL(CLASS) \
    (((srlz::CONCEPTS_MEMBER_EXISTS(CLASS, serialize_size) || \
       srlz::CONCEPTS_MEMBER_EXISTS(CLASS, c_serialize_size)) && \
       srlz::CONCEPTS_MEMBER_EXISTS(CLASS, serialize_impl)) || \
       concepts::numbers<T>)




namespace srlz {
    CONCEPTS_MEMBER_CHECKER(c_serialize_size);
    CONCEPTS_MEMBER_CHECKER(serialize_size);
    CONCEPTS_MEMBER_CHECKER(serialize_impl);
    CONCEPTS_MEMBER_CHECKER(deserialize_impl);

    // Todo: unrolling
    // Serialize
    template <typename T>
    auto _serialize_tmpl(Byte* buf, T num) -> std::enable_if_t<concepts::integers<T>> {
        constexpr auto size = sizeof(T);
        for (SizeT i = 0; i < size; ++i)
            *(buf + i) =
                static_cast<Byte>(
                    static_cast<
                        std::make_unsigned_t<T>>
                            (num) >> ((size - i - 1) << 3));
    }
    template <typename T>
    auto _serialize_tmpl(Byte* buf, T num) -> std::enable_if_t<concepts::floats<T>> {
        using UintT = std::conditional_t<std::is_same_v<T, Float64>, U64, U32>;
        constexpr auto size = sizeof(T);

        for (SizeT i = 0; i < size; ++i)
            *(buf + i) =
                static_cast<Byte>(
                    *reinterpret_cast<UintT*>
                        (&num) >> ((size - i - 1) << 3));
    }
    template <typename T>
    auto _serialize_tmpl(Byte* buf, const T& val)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, serialize_impl)> {
        val.serialize_impl(buf);
    }

    // Deserialize
    template <typename T>
    auto _deserialize_tmpl(T& num, const Byte* buf) -> std::enable_if_t<concepts::integers<T>> {
        constexpr auto size = sizeof(T);
        num = static_cast<T>(*buf);
        if constexpr (size > 1) {
            for (SizeT i = 1; i < size; ++i) {
                num <<= 8;
                num |= static_cast<T>(*(buf + i));
            }
        }
    }
    template <typename T>
    auto _deserialize_tmpl(T& num, const Byte* buf) -> std::enable_if_t<concepts::floats<T>> {
        using UintT = std::conditional_t<std::is_same_v<T, Float64>, U64, U32>;
        constexpr auto size = sizeof(T);
        auto ptr = reinterpret_cast<UintT*>(&num);
        *ptr = static_cast<UintT>(*buf);

        for (SizeT i = 1; i < size; ++i) {
            *ptr <<= 8;
            *ptr |= static_cast<UintT>(*(buf + i));
        }
    }
    template <typename T>
    auto _deserialize_tmpl(T& val, const Byte* buf)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, deserialize_impl)> {
        val.deserialize_impl(buf);
    }

    template <typename T>
    inline constexpr auto _const_sizeof_tmpl()
    -> std::enable_if_t<concepts::numbers<T>, SizeT> {
        return sizeof(T);
    }
    template <typename T>
    inline constexpr auto _const_sizeof_tmpl()
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, c_serialize_size), SizeT> {
        constexpr auto size = T::c_serialize_size();
        return size;
    }

    template <typename T>
    inline constexpr auto _sizeof_tmpl(const T&)
    -> std::enable_if_t<concepts::numbers<T>, SizeT> {
        return sizeof(T);
    }
    template <typename T>
    inline constexpr auto _sizeof_tmpl(const T& v)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, c_serialize_size), SizeT> {
        return v.c_serialize_size();
    }
    template <typename T>
    inline constexpr auto _sizeof_tmpl(const T& v)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, serialize_size), SizeT> {
        return v.serialize_size();
    }


    // Serialize

    // static elements
    template <typename T>
    auto _serialize_array_tmpl(Byte* buf, const T* array, SizeT count, SizeT itemSize) {
        if constexpr (sizeof(T) == 1) { // Todo: Is there a one byte class with defined serialize methods?
            memcpy(buf, array, count);
        } else {
            repeat(count) {
                _serialize_tmpl(buf, *array++);
                buf += itemSize;
            }
        }
    }
    // dynamic elements
    template <typename T>
    auto _serialize_array_tmpl(Byte* buf, const T* array,
                               [[maybe_unused]]SizeT count,
                               const std::pair<ftl::Vector<SizeT>, SizeT>& itemSizes)
    {
        for (auto& size : itemSizes.first) {
            _serialize_tmpl(buf, *array++);
            buf += size;
        }
    }

    // Deserialize
    template <typename T>
    auto _deserialize_array_tmpl(T* array, SizeT size, const Byte* buf) {
        if constexpr (sizeof(T) == 1) { // Todo: Is there a one byte class with defined serialize methods?
            memcpy(array, buf, size);
            return size;
        } else {
            SizeT realSize = 0;
            for (SizeT i = 0; i < size; ++i) {
                _deserialize_tmpl(*(array + i), buf);
                auto sz = _sizeof_tmpl(*(array + i));
                buf += sz;
                realSize += sz;
            }
            return realSize;
        }
    }

    template <typename T>
    inline constexpr auto _sizeof_array_tmpl(const T*, SizeT size)
    -> std::enable_if_t<concepts::numbers<T>, SizeT> {
        return sizeof(T);
    }
    template <typename T>
    inline constexpr auto _sizeof_array_tmpl(const T* v, SizeT size)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, c_serialize_size), SizeT> {
        return T::c_serialize_size();
    }
    template <typename T>
    inline auto _sizeof_array_tmpl(const T* v, SizeT size)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, serialize_size), std::pair<ftl::Vector<SizeT>, SizeT>> {
        using ReturnT = std::pair<ftl::Vector<SizeT>, SizeT>;
        auto itemSizes = ReturnT(ftl::Vector<SizeT>(size, 0), 0);

        for (auto& item : itemSizes.first) {
            item = (v++)->serialize_size();
            itemSizes.second += item;
        }

        return std::move(itemSizes);
    }
    inline SizeT _sizeof_array_unpacker(const std::pair<ftl::Vector<SizeT>, SizeT>& p, SizeT size) {
        return p.second;
    }
    inline SizeT _sizeof_array_unpacker(SizeT s, SizeT size) {
        return s * size;
    }



    template <typename T>
    auto serialize(const T& val) {
        /*static_assert(
                concepts::numbers<T> || (
                CONCEPTS_MEMBER_EXISTS(T, c_serialize_size) ||
                CONCEPTS_MEMBER_EXISTS(T, serialize_size)),
                "Can't find c_serialize_size or serialize_size member!");

        static_assert(
                concepts::numbers<T> || (
                CONCEPTS_MEMBER_EXISTS(T, serialize_impl)),
                "Can't find serialize_impl member");
                */

        if constexpr (concepts::numbers<T> || CONCEPTS_MEMBER_EXISTS(T, c_serialize_size)) {
            constexpr auto size = _const_sizeof_tmpl<T>();
            auto buf = ftl::Array<Byte, size>();
            _serialize_tmpl(buf.data(), val);
            return std::move(buf);
        }
        else if constexpr (CONCEPTS_MEMBER_EXISTS(T, serialize_size)) {
            auto buf = ftl::Vector<Byte>(_sizeof_tmpl(val), Byte(0));
            _serialize_tmpl(buf.data(), val);
            return std::move(buf);
        }
    }

    template <typename T>
    auto serialize(const T* array, SizeT count) {
        auto realSize  = _sizeof_array_tmpl(array, count);
        auto buf       = ftl::Vector<Byte>();

        if constexpr (std::is_same_v<decltype(realSize), SizeT>)
            buf.resize(realSize * count, Byte(0));
        else
            buf.resize(realSize.second, Byte(0));

        _serialize_array_tmpl(buf.data(), array, count, realSize);

        return std::move(buf);
    }

    template <typename T>
    auto deserialize(T& val, const Byte* data) {
        _deserialize_tmpl(val, data);
        return _sizeof_tmpl(val);
    }

    template <typename T>
    auto deserialize(T* array, SizeT size, const Byte* data) {
        return _deserialize_array_tmpl(array, size, data);
    }
}

#endif //DECAYENGINE_SERIALIZATION_HPP
