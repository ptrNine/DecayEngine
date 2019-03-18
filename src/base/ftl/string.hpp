#ifndef DECAYENGINE_STRING_HPP
#define DECAYENGINE_STRING_HPP

#include <ostream>
#include <string_view>
#include <utility>
#include <cstring>
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
        SCA hash() {
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
        ftl::const_str_detail::buildHelper( \
            (STR)[0], \
            []{ struct Arr { static constexpr auto get() { return STR; } }; return Arr{}; }(), \
            std::make_index_sequence<sizeof((STR)) / sizeof((STR)[0]) - 1>{})

    }



    template <typename CharT>
    class StringBase {
        static_assert(
                concepts::any_of<CharT, Char8, Char16, Char32>,
                "String must contains char symbols only"
        );

        using SizeType = typename std::basic_string<CharT>::size_type;

    public:
        StringBase() = default;

        StringBase(const StringBase& str)                           : _str_v(str._str_v) {}
        StringBase(const StringBase& str, SizeType pos)             : _str_v(str._str_v, pos) {}
        StringBase(const StringBase& str, SizeType pos, SizeType n) : _str_v(str._str_v, pos, n) {}

        explicit StringBase(const std::basic_string<CharT>& str)      : _str_v(str) {}
        explicit StringBase(const std::basic_string_view<CharT>& str) : _str_v(str) {}
        explicit StringBase(std::basic_string<CharT>&& str)           : _str_v(std::move(str)) {}
        explicit StringBase(std::basic_string_view<CharT>&& str)      : _str_v(std::move(str)) {}

        explicit
        StringBase(const CharT* str)                                : _str_v(str) {}
        StringBase(const CharT* str, SizeType n)                    : _str_v(str, n) {}
        StringBase(SizeType n, CharT c)                             : _str_v(c, n) {}
        StringBase(std::initializer_list<CharT> l)                  : _str_v(l) {}
        StringBase(StringBase&& str) noexcept                       : _str_v(std::move(str._str_v)) {}

        template<typename InputIterator>
        StringBase(InputIterator beg, InputIterator end)            : _str_v(beg, end) {}

        template <std::size_t _Size>
        explicit StringBase(const CharT(&str)[_Size])               : _str_v(str) {}

        template <CharT... _Str>
        explicit StringBase(ConstexprString<CharT, _Str...> str)    : StringBase(str.str_view()) {}

        inline auto max_size() const { return _str_v.max_size(); }
        inline auto lenght  () const { return _str_v.length(); }
        inline auto capacity() const { return _str_v.capacity(); }
        inline auto empty   () const { return _str_v.empty(); }
        inline auto size    () const { return _str_v.size(); }
        inline auto c_str   () const { return _str_v.c_str(); }
        inline auto data    () const { return _str_v.data(); }
        inline auto data    ()       { return _str_v.data(); }
        inline auto clear   ()       { _str_v.clear(); return *this; }

        inline auto resize  (SizeType size)          { _str_v.resize(size); return *this; }
        inline auto resize  (SizeType size, CharT c) { _str_v.resize(size, c); return *this; }
        inline auto reserve (SizeType size)          { _str_v.reserve(size); return *this; }

        inline auto at    (SizeType pos) -> CharT&             { return _str_v.at(pos); }
        inline auto at    (SizeType pos) const -> const CharT& { return _str_v.at(pos); }
        inline auto front () -> CharT&                         { return _str_v.front(); }
        inline auto front () const -> const CharT&             { return _str_v.front(); }
        inline auto back  () -> CharT&                         { return _str_v.back(); }
        inline auto back  () const -> const CharT&             { return _str_v.back(); }

        inline auto shrink_to_fit()     { _str_v.shrink_to_fit(); return *this; }


        // Iterators
        inline auto begin   ()       { return _str_v.begin(); }
        inline auto end     ()       { return _str_v.end(); }
        inline auto rbegin  ()       { return _str_v.rbegin(); }
        inline auto rend    ()       { return _str_v.rend(); }
        inline auto cbegin  () const { return _str_v.cbegin(); }
        inline auto cend    () const { return _str_v.cend(); }
        inline auto crbegin () const { return _str_v.crbegin(); }
        inline auto crend   () const { return _str_v.crend(); }


        // Operators
        inline auto operator==(const StringBase& str) const { return _str_v == str._str_v; }
        inline auto operator!=(const StringBase& str) const { return _str_v != str._str_v; }

        template <SizeType _Size>
        inline auto operator==(const CharT(&str)[_Size]) const { return _str_v == str; }
        template <SizeType _Size>
        inline auto operator!=(const CharT(&str)[_Size]) const { return _str_v != str; }

        template <CharT... _Str>
        inline auto operator==(ConstexprString<CharT, _Str...> str) const {
            constexpr auto strview = str.str_view();
            return _str_v == strview;
        }
        template <CharT... _Str>
        inline auto operator!=(ConstexprString<CharT, _Str...> str) const {
            constexpr auto strview = str.str_view();
            return _str_v != strview;
        }

        inline auto operator+(const StringBase& str) const    { return StringBase(_str_v + str._str_v); }
        inline auto operator+(CharT c) const                  { return StringBase(_str_v + c); }
        template <SizeType _Size>
        inline auto operator+(const CharT(&str)[_Size]) const { return StringBase(_str_v + str); }
        template <CharT... _Str>
        inline auto operator+(ConstexprString<CharT, _Str...> str) const {
            return StringBase(_str_v + str.c_str());
        }

        inline auto operator+=(const StringBase& str) -> StringBase& { _str_v += str._str_v; return *this; }
        inline auto operator+=(CharT c) -> StringBase&               { _str_v += c; return *this; }
        template <SizeType _Size>
        inline auto operator+=(const CharT(&str)[_Size]) -> StringBase& { _str_v += str; return *this; }
        template <CharT... _Str>
        inline auto operator+=(ConstexprString<CharT, _Str...> str) -> StringBase& {
            constexpr auto strview = str.str_view();
            _str_v += strview;
            return *this;
        }

        inline auto operator/(const StringBase& str) const
        {
            if (_str_v.empty() || str.empty())
                return *this + str;
            else {
                if (_str_v.back() == '/' && str.front() == '/')
                    return StringBase(_str_v + (str.data() + 1));
                else if (_str_v.back() == '/' || str.front() == '/')
                    return StringBase(*this + str);
                else
                    return StringBase(*this + '/' + str);
            }
        }
        template <SizeType _Size>
        inline auto operator/(const CharT(&str)[_Size]) const
        {
            if (_str_v.empty() || _Size < 1)
                return *this + str;
            else {
                if (_str_v.back() == '/' && str[0] == '/')
                    return StringBase(_str_v + &str[1]);
                else if (_str_v.back() == '/' || str[0] == '/')
                    return StringBase(*this + str);
                else
                    return StringBase(*this + '/' + str);
            }
        }
        template <CharT... _Str>
        inline auto operator/(ConstexprString<CharT, _Str...> str) const
        {
            if (_str_v.empty() || sizeof...(_Str) == 0)
                return StringBase(_str_v + str.c_str());
            else {
                if (_str_v.back() == '/' && str[0] == '/')
                    return StringBase(_str_v + str.template cut_l<1>().c_str());
                else if (_str_v.back() == '/' || str[0] == '/')
                    return StringBase(_str_v + str.c_str());
                else
                    return StringBase((_str_v + const_str_detail::delim<CharT>()) + str.c_str());
            }
        }

        inline auto operator/=(const StringBase& str) -> StringBase&
        {
            if (_str_v.empty() || str.empty())
                *this += str;
            else {
                if (_str_v.back() == '/' && str.front() == '/')
                    _str_v += (str.data() + 1);
                else if (_str_v.back() == '/' || str.front() == '/')
                    *this += str;
                else
                    (*this += '/') += str;
            }
            return *this;
        }
        template <SizeType _Size>
        inline auto operator/=(const CharT(&str)[_Size]) -> StringBase&
        {
            if (_str_v.empty() || _Size < 1)
                _str_v += str;
            else {
                if (_str_v.back() == '/' && str[0] == '/')
                    _str_v += &str[1];
                else if (_str_v.back() == '/' || str[0] == '/')
                    _str_v += str;
                else
                    (_str_v += '/') += str;
            }
            return *this;
        }
        template <CharT... _Str>
        inline auto operator/=(ConstexprString<CharT, _Str...> str) -> StringBase&
        {
            if (_str_v.empty() || sizeof...(_Str) == 0)
                _str_v += str.c_str();
            else {
                if (_str_v.back() == '/' && str[0] == '/')
                    _str_v += str.template cut_l<1>().c_str();
                else if (_str_v.back() == '/' || str[0] == '/')
                    _str_v += str.c_str();
                else
                    (_str_v += '/') += str.str_view();
            }
            return *this;
        }

        inline auto operator[](SizeType pos)       -> CharT&       { return _str_v[pos]; }
        inline auto operator[](SizeType pos) const -> const CharT& { return _str_v[pos]; }

    protected:
        std::basic_string<CharT> _str_v;
    };

    using String   = StringBase<Char8>;
    using String16 = StringBase<Char16>;
    using String32 = StringBase<Char32>;

} // namespace ftl


#undef SCA  // static constexpr auto

#endif //DECAYENGINE_STRING_HPP
