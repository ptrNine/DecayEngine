#ifndef DECAYENGINE_CP_STRING_HPP
#define DECAYENGINE_CP_STRING_HPP

#include <fmt/format.h>
extern "C" {
#include <xxhash.h>
}
#include <utility>
#include <ostream>
#include <string_view>
#include "../baseTypes.hpp"
#include "../concepts.hpp"

#define SCA static constexpr auto

namespace ftl {
    namespace const_str_detail {
        template<typename CharT, CharT... _Str>
        constexpr CharT const storage[sizeof...(_Str) + 1] = {_Str..., CharT(0)};

        template <typename CharT>
        constexpr const CharT* delim();

        template <>
        constexpr const Char8* delim() { return "/"; }

        template <>
        constexpr const Char16* delim() { return u"/"; }

        template <>
        constexpr const Char32* delim() { return U"/"; }

    }

    /**
     * @brief Compile-time string implementation.
     *
     * Support char, char16_t and char32_t strings. <br>
     * For creating use CS macro: <br><br>
     *     auto str   = CS("More compile time shit please"); <br>
     *     auto str16 = CS(u"Abcdefj"); <br><br>
     *     // prints "More compile time shit please!!!!!!" <br>
     *     std::cout << str + CS("!!!!!!") << std::endl;
     */
    template<typename CharT, CharT... _Str>
    struct ConstexprString {
        static_assert(
                concepts::any_of<CharT, Char8, Char16, Char32>,
                "String must contains char symbols only"
        );

        /// @return size of constant string
        SCA size() { return sizeof...(_Str); }

        /// @return C-style string
        SCA c_str() {
            return &const_str_detail::storage<CharT, _Str...>[0];
        }

        /// @return std::base_string_view string
        SCA str_view() {
            return std::basic_string_view<CharT>{c_str()};
        }

        /**
         * Getting symbol by pos with compile-time bounds check
         * @tparam _Pos - symbol position
         * @return symbol
         */
        template <std::size_t _Pos>
        SCA get() {
            static_assert(_Pos < sizeof...(_Str), "Index out of bounds");
            return const_str_detail::storage<CharT, _Str...>[_Pos];
        }

        SCA front() {
            return const_str_detail::storage<CharT, _Str...>[0];
        }

        SCA back() {
            constexpr auto size = sizeof...(_Str);
            return const_str_detail::storage<CharT, _Str...>[size == 0 ? 0 : size - 1];
        }

        template <std::size_t _Pos, std::size_t ...idx>
        SCA cut_l(std::index_sequence<idx...>) {
            return ftl::ConstexprString<CharT, const_str_detail::storage<CharT, _Str...>[idx + _Pos]...>{};
        }
        /**
         * Truncate left side before position
         * @tparam _Pos - position
         * @return truncated constexpr string
         */
        template <std::size_t _Pos>
        SCA cut_l() {
            return cut_l<_Pos>(std::make_index_sequence<sizeof...(_Str) - _Pos>{});
        }


        template <std::size_t _Pos, std::size_t ...idx>
        SCA cut_r(std::index_sequence<idx...>) {
            return ftl::ConstexprString<CharT, const_str_detail::storage<CharT, _Str...>[idx]...>{};
        }
        /**
         * Truncate left side after position
         * @tparam _Pos - position
         * @return truncated constexpr string
         */
        template <std::size_t _Pos>
        SCA cut_r() {
            return cut_r<_Pos>(std::make_index_sequence<sizeof...(_Str) - _Pos>{});
        }

        /**
         * Hash value depends on size of char symbol!
         * @return 64-bit FNV-1a hash
         */
        SCA chash() {
            U64 hsh = 14695981039346656037ULL;
            if constexpr (std::is_same_v<CharT, Char8>) {
                (((hsh ^= _Str) *= 1099511628211ULL), ...);
            }
            else if constexpr (std::is_same_v<CharT, Char16>) {
                ((
                        ((hsh ^= (_Str >> 8     )) *= 1099511628211ULL),
                                ((hsh ^= (_Str &  0x00FF)) *= 1099511628211ULL)
                ), ...);
            }
            else if constexpr (std::is_same_v<CharT, Char32>) {
                ((
                        ((hsh ^= ( _Str >> 24        )) *= 1099511628211ULL),
                        ((hsh ^= ((_Str >> 16) & 0xFF)) *= 1099511628211ULL),
                        ((hsh ^= ((_Str >> 8 ) & 0xFF)) *= 1099511628211ULL),
                        ((hsh ^= ( _Str        & 0xFF)) *= 1099511628211ULL)
                ), ...);
            }
            return hsh;
        }

        static U64 hash() {
            return XXH64(c_str(), size() * sizeof(CharT), 0);
        }


        // Operators

        constexpr auto operator[](std::size_t&& pos) const {
            return const_str_detail::storage<CharT, _Str...>[pos];
        }

        template <CharT... Rhs>
        constexpr auto operator+ (ConstexprString<CharT, Rhs...>) const {
            return ConstexprString<CharT, _Str..., Rhs...>();
        }

        template <CharT... _Rhs>
        constexpr auto operator/ (ConstexprString<CharT, _Rhs...>) const {
            if constexpr (
                    const_str_detail::storage<CharT, _Str...>[sizeof...(_Str) - 1] != '/' &&
                    const_str_detail::storage<CharT, _Rhs...>[0] != '/'
                    )
                return ConstexprString<CharT, _Str..., CharT('/'), _Rhs...>();
            else if constexpr (
                    const_str_detail::storage<CharT, _Str...>[sizeof...(_Str) - 1] == '/' &&
                    const_str_detail::storage<CharT, _Rhs...>[0] == '/'
                    )
                return ConstexprString<CharT, _Str...>() + ConstexprString<CharT, _Rhs...>().template cut_l<1>();
            else
                return ConstexprString<CharT, _Str..., _Rhs...>();
        }

        template <CharT... _Rhs>
        constexpr bool operator== (const ConstexprString<CharT, _Rhs...>) const {
            if constexpr (sizeof...(_Str) != sizeof...(_Rhs))
                return false;
            else
                return ((_Str == _Rhs) && ...);
        }

        template <CharT... _Rhs>
        constexpr bool operator!= (const ConstexprString<CharT, _Rhs...>) const {
            return !(operator==(ConstexprString<CharT, _Rhs...>{}));
        }

        friend std::ostream& operator<< (std::ostream& os, const ConstexprString<CharT, _Str...>) {
            os << ConstexprString<CharT, _Str...>::c_str();
            return os;
        }
    };


    namespace const_str_detail {
        template<typename CharT, typename Arr, std::size_t ...idx>
        constexpr ConstexprString<CharT, Arr::get()[idx]...>
        buildHelper(CharT, Arr, std::index_sequence<idx...>) {
            return {};
        }

        #define CS(STR) \
        (ftl::const_str_detail::buildHelper( \
            (STR)[0], \
            []{ struct Arr { static constexpr auto get() { return STR; } }; return Arr{}; }(), \
            std::make_index_sequence<sizeof((STR)) / sizeof((STR)[0]) - 1>{}))

    }
} // namespace ftl

// fmt format
template <Char8... _Str>
struct fmt::formatter<ftl::ConstexprString<Char8, _Str...>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ftl::ConstexprString<Char8, _Str...>& str, FormatContext& ctx) {
        return format_to(ctx.out(), "{}", str.c_str());
    }
};

// std chash
template <typename CharT, CharT... _Str>
struct std::hash<ftl::ConstexprString<CharT, _Str...>> {
    U64 operator()(const ftl::ConstexprString<CharT, _Str...>& str) const {
        return str.hash();
    }
};


#undef SCA  // static constexpr auto

#endif //DECAYENGINE_CP_STRING_HPP
