#ifndef DECAYENGINE_SERIALIZATION_HPP
#define DECAYENGINE_SERIALIZATION_HPP
#include "ftl/array.hpp"
#include "ftl/vector.hpp"
#include "lang_defines.hpp"


namespace concepts {
    namespace detail_srlz {
        template<typename T,
                decltype(std::declval<T>().serialize(std::declval<Byte*>()))...>
        struct srlz {
            static constexpr bool value =
                    std::is_same_v<decltype(std::declval<T>().serialize_size()), SizeT>;
        };

        template<>
        struct srlz<ftl::Vector<Byte>> {
            static constexpr bool value = true;
        };

        template<SizeT _Size>
        struct srlz<ftl::Array<Byte, _Size>> {
            static constexpr bool value = true;
        };
    }

    template<typename T>
    constexpr bool serializable = detail_srlz::srlz<T>::value;
}

#define SERIALIZE_METHOD_CONST_SIZE() \
    constexpr auto c_serialize_size() const

#define SERIALIZE_METHOD_SIZE() \
    auto serialize_size() const

#define SERIALIZE_METHOD() \
    void serialize_impl(Byte* _serializer, SizeT _serializer_pos = 0) const

#define SERIALIZE_START(BUFFER) SizeT _serializer_pos = 0; auto _serializer = (BUFFER)

#define SERIALIZE(VALUE) \
    srlz::_serialize_tmpl(_serializer + _serializer_pos, (VALUE)); \
    _serializer_pos += srlz::_sizeof_tmpl(VALUE)

#define SERIALIZE_ARRAY(PTR, SIZE) { \
    auto temp = srlz::_sizeof_array_tmpl(PTR, SIZE); \
    srlz::_serialize_array_tmpl(_serializer + _serializer_pos, PTR, SIZE, temp); \
    _serializer_pos += srlz::_sizeof_array_unpacker(temp, SIZE); \
}

#define SERIALIZE_GET_SIZE_ARRAY(PTR, SIZE) \
    srlz::_sizeof_array_unpacker(srlz::_sizeof_array_tmpl((PTR), (SIZE)), SIZE)

#define SERIALIZE_GET_SIZE(VALUE) \
    srlz::_sizeof_tmpl(VALUE)


namespace srlz {
    CONCEPTS_MEMBER_CHECKER(c_serialize_size);
    CONCEPTS_MEMBER_CHECKER(serialize_size);
    CONCEPTS_MEMBER_CHECKER(serialize_impl);

    // Todo: unrolling
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

    template <typename T>
    inline constexpr auto _sizeof_array_tmpl(const T*, SizeT size)
    -> std::enable_if_t<concepts::numbers<T>, SizeT> {
        return sizeof(T);
    }
    template <typename T>
    inline constexpr auto _sizeof_array_tmpl(const T* v, SizeT size)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, c_serialize_size), SizeT> {
        return v->c_serialize_size();
    }
    template <typename T>
    inline auto _sizeof_array_tmpl(const T* v, SizeT size)
    -> std::enable_if_t<CONCEPTS_MEMBER_EXISTS(T, serialize_size), std::pair<ftl::Vector<SizeT>, SizeT>> {
        using ReturnT = std::pair<ftl::Vector<SizeT>, SizeT>;
        auto itemSizes = ReturnT(ftl::Vector<SizeT>(size, 0), 0);

        for (auto& item : itemSizes.first) {
            auto sz = (v++)->serialize_size();
            item = sz;
            itemSizes.second += sz;
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
            constexpr auto size = _sizeof_tmpl(val);
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
}

#endif //DECAYENGINE_SERIALIZATION_HPP
