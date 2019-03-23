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


#define SERIALIZE_START(BUFFER) SizeT _serializer_pos = 0; auto _serializer = (BUFFER)

#define SERIALIZE(VALUE) \
    srlz::_serialize_tmpl(_serializer + _serializer_pos, (VALUE)); \
    _serializer_pos += srlz::_sizeof_tmpl(VALUE)



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




    template <typename T>
    auto serialize(const T& val) {
        static_assert(
                concepts::numbers<T> || (
                CONCEPTS_MEMBER_EXISTS(T, c_serialize_size) ||
                CONCEPTS_MEMBER_EXISTS(T, serialize_size)),
                "Can't find c_serialize_size or serialize_size member!");

        static_assert(
                concepts::numbers<T> || (
                CONCEPTS_MEMBER_EXISTS(T, serialize_impl)),
                "Can't find serialize_impl member");

        if constexpr (CONCEPTS_MEMBER_EXISTS(T, c_serialize_size) || concepts::numbers<T>) {
            constexpr auto size = _sizeof_tmpl(val);
            auto buf = ftl::Array<Byte, size>();
            _serialize_tmpl(buf.data(), val);
            return buf;
        }
        else if constexpr (CONCEPTS_MEMBER_EXISTS(T, serialize_size)) {
            auto buf = ftl::Vector<Byte>(_sizeof_tmpl(val), Byte(0));
            _serialize_tmpl(buf.data(), val);
            return buf;
        }
    }

}

#endif //DECAYENGINE_SERIALIZATION_HPP
