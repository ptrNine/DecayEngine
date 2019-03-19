#ifndef DECAYENGINE_STRING_HPP
#define DECAYENGINE_STRING_HPP

#include "cp_string.hpp"
#include "../baseTypes.hpp"
#include "../concepts.hpp"

namespace ftl {
    template <typename CharT>
    class StringBase {
        static_assert(
                concepts::any_of<CharT, Char8, Char16, Char32>,
                "String must contains char symbols only"
        );

        using SizeType = typename std::basic_string<CharT>::size_type;
        using IterT    = typename std::basic_string<CharT>::iterator;
        using C_IterT  = typename std::basic_string<CharT>::const_iterator;
        using R_IterT  = typename std::basic_string<CharT>::const_iterator;
        using CR_IterT = typename std::basic_string<CharT>::const_reverse_iterator;

    public:
        static constexpr auto npos = std::basic_string<CharT>::npos;


        // Constructors
        StringBase() = default;

        StringBase(const StringBase& str)                           : _str_v(str._str_v) {}
        StringBase(const StringBase& str, SizeType pos)             : _str_v(str._str_v, pos) {}
        StringBase(const StringBase& str, SizeType pos, SizeType n) : _str_v(str._str_v, pos, n) {}

        explicit StringBase(const std::basic_string<CharT>& str)      : _str_v(str) {}
        explicit StringBase(const std::basic_string_view<CharT>& str) : _str_v(str) {}
        explicit StringBase(std::basic_string<CharT>&& str)           : _str_v(std::move(str)) {}
        explicit StringBase(std::basic_string_view<CharT>&& str)      : _str_v(std::move(str)) {}

        StringBase(SizeType n, CharT c)            : _str_v(c, n) {}
        StringBase(std::initializer_list<CharT> l) : _str_v(l) {}
        StringBase(StringBase&& str) noexcept      : _str_v(std::move(str._str_v)) {}

        template<typename InputIterator>
        StringBase(InputIterator beg, InputIterator end) : _str_v(beg, end) {}

        template <std::size_t _Size>
        explicit StringBase(const CharT(&str)[_Size]) : _str_v(str, _Size ? _Size-1 : 0) {}

        template <CharT... _Str>
        explicit StringBase(ConstexprString<CharT, _Str...> str) : StringBase(str.str_view()) {}


        // Basic methods

        inline auto max_size() const { return _str_v.max_size(); }
        inline auto lenght  () const { return _str_v.length(); }
        inline auto capacity() const { return _str_v.capacity(); }
        inline auto empty   () const { return _str_v.empty(); }
        inline auto size    () const { return _str_v.size(); }
        inline auto c_str   () const { return _str_v.c_str(); }
        inline auto data    () const { return _str_v.data(); }
        inline auto data    ()       { return _str_v.data(); }
        inline auto clear   ()       { _str_v.clear(); return *this; }

        inline auto resize  (SizeType size)          -> StringBase& { _str_v.resize(size); return *this; }
        inline auto resize  (SizeType size, CharT c) -> StringBase& { _str_v.resize(size, c); return *this; }
        inline auto reserve (SizeType size)          -> StringBase& { _str_v.reserve(size); return *this; }

        inline auto at    (SizeType pos) -> CharT&             { return _str_v.at(pos); }
        inline auto at    (SizeType pos) const -> const CharT& { return _str_v.at(pos); }
        inline auto front () -> CharT&                         { return _str_v.front(); }
        inline auto front () const -> const CharT&             { return _str_v.front(); }
        inline auto back  () -> CharT&                         { return _str_v.back(); }
        inline auto back  () const -> const CharT&             { return _str_v.back(); }

        inline auto shrink_to_fit()     { _str_v.shrink_to_fit(); return *this; }

        inline auto push_back(CharT c) -> StringBase& { _str_v.push_back(c); return *this; }
        inline auto pop_back ()        -> StringBase& { _str_v.pop_back(); return *this; }

        inline void swap    (StringBase& str) { _str_v.swap(str._str_v); }


        // Iterators

        inline auto begin   ()       { return _str_v.begin(); }
        inline auto end     ()       { return _str_v.end(); }
        inline auto rbegin  ()       { return _str_v.rbegin(); }
        inline auto rend    ()       { return _str_v.rend(); }
        inline auto cbegin  () const { return _str_v.cbegin(); }
        inline auto cend    () const { return _str_v.cend(); }
        inline auto crbegin () const { return _str_v.crbegin(); }
        inline auto crend   () const { return _str_v.crend(); }
        inline auto begin   () const { return _str_v.begin(); }
        inline auto end     () const { return _str_v.end(); }
        inline auto rbegin  () const { return _str_v.rbegin(); }
        inline auto rend    () const { return _str_v.rend(); }

        inline auto assign_c_str(const CharT* str) -> StringBase& {
            _str_v.assign(str);
            return *this;
        }

        inline auto assign_c_str(const CharT* str, SizeType n) -> StringBase& {
            _str_v.assign(str, n);
            return *this;
        }

        // Algorithms

        inline auto insert(C_IterT start, SizeType n, CharT c) {
            return _str_v.insert(start, n, c);
        }
        template <typename InputIterT>
        inline auto insert(C_IterT start, InputIterT beg, InputIterT end) {
            return _str_v.insert(start, beg, end);
        }
        inline auto insert(C_IterT start, CharT c) {
            return _str_v.insert(start, c);
        }
        inline void insert(IterT start, std::initializer_list<CharT> l) {
            _str_v.insert(start, l);
        }
        inline auto insert(SizeType pos, const StringBase& str) -> StringBase& {
            _str_v.insert(pos, str._str_v);
            return *this;
        }
        inline auto insert(SizeType pos, const StringBase& str,
                           SizeType pos2, SizeType n = npos) -> StringBase& {
            _str_v.insert(pos, str._str_v, pos2, n);
            return *this;
        }
        template<SizeType _Size>
        inline auto insert(SizeType pos, const CharT(&str)[_Size]) -> StringBase& {
            _str_v.insert(pos, str, _Size ? _Size-1 : 0);
            return *this;
        }
        template<SizeType _Size>
        inline auto insert(SizeType pos, const CharT(&str)[_Size],
                           SizeType pos2, SizeType n) -> StringBase& {
            _str_v.insert(pos, &str[0] + pos2, n);
            return *this;
        }
        template<CharT... _Str>
        inline auto insert(SizeType pos, const ConstexprString<CharT, _Str...> str) -> StringBase& {
            _str_v.insert(pos, str.c_str(), sizeof...(_Str));
            return *this;
        }
        template<CharT... _Str>
        inline auto insert(SizeType pos, const ConstexprString<CharT, _Str...> str,
                           SizeType pos2, SizeType n
        ) -> StringBase& {
            _str_v.insert(pos, str.c_str(), pos2, n);
            return *this;
        }
        inline auto insert(SizeType pos, SizeType n, CharT c) -> StringBase& {
            _str_v.insert(pos, n, c);
            return *this;
        }

        inline auto erase(SizeType pos, SizeType n = npos) -> StringBase& {
            _str_v.erase(pos, n);
            return *this;
        }
        inline auto erase(C_IterT pos) {
            return _str_v.erase(pos);
        }
        inline auto erase(C_IterT first, C_IterT last) {
            return _str_v.erase(first, last);
        }

        inline auto replace(SizeType pos, SizeType n, const StringBase& str) -> StringBase& {
            _str_v.replace(pos, n, str._str_v);
            return *this;
        }
        inline auto replace(SizeType pos,  SizeType n, const StringBase& str,
                            SizeType pos2, SizeType n2 = npos) -> StringBase& {
            _str_v.replace(pos, n, str._str_v, pos2, n2);
            return *this;
        }
        template <SizeType _Size>
        inline auto replace(SizeType pos, SizeType n, const CharT (&str)[_Size],
                            SizeType pos2, SizeType n2) -> StringBase& {
            _str_v.replace(pos, n, &str[0] + pos2, n2);
            return *this;
        }
        template <CharT... _Str>
        inline auto replace(SizeType pos, SizeType n, const ConstexprString<CharT, _Str...> str,
                            SizeType pos2, SizeType n2) -> StringBase& {
            _str_v.replace(pos, n, str.c_str() + pos2, n2);
            return *this;
        }
        inline auto replace(SizeType pos, SizeType n1, SizeType n2, CharT c) -> StringBase& {
            _str_v.replace(pos, n1, n2, c);
            return *this;
        }
        inline auto replace(C_IterT i1, C_IterT i2, const StringBase& str) -> StringBase& {
            _str_v.replace(i1, i2, str._str_v);
            return *this;
        }
        template <SizeType _Size>
        inline auto replace(C_IterT i1, C_IterT i2, const CharT (&str)[_Size],
                            SizeType n2 = _Size ? _Size-1 : 0) -> StringBase& {
            _str_v.replace(i1, i2, &str[0], n2);
            return *this;
        }
        template <SizeType _Size>
        inline auto replace(C_IterT i1, C_IterT i2, const CharT (&str)[_Size],
                            SizeType pos, SizeType n2) -> StringBase& {
            _str_v.replace(i1, i2, &str[0] + pos, n2);
            return *this;
        }
        template <CharT... _Str>
        inline auto replace(C_IterT i1, C_IterT i2, const ConstexprString<CharT, _Str...> str,
                            SizeType n2 = sizeof...(_Str)) -> StringBase& {
            _str_v.replace(i1, i2, str.c_str(), n2);
            return *this;
        }
        template <CharT... _Str>
        inline auto replace(C_IterT i1, C_IterT i2, const ConstexprString<CharT, _Str...> str,
                            SizeType pos, SizeType n2) -> StringBase& {
            _str_v.replace(i1, i2, str.c_str() + pos, n2);
            return *this;
        }
        inline auto replace(C_IterT i1, C_IterT i2, SizeType n2, CharT c) -> StringBase& {
            _str_v.replace(i1, i2, n2, c);
            return *this;
        }
        template <typename InputIterT>
        inline auto replace(C_IterT i1, C_IterT i2, InputIterT s1, InputIterT s2) -> StringBase& {
            _str_v.replace(i1, i2, s1, s2);
            return *this;
        }
        inline auto replace(C_IterT i1, C_IterT i2, std::initializer_list<CharT> l) -> StringBase& {
            _str_v.replace(i1, i2, l);
            return *this;
        }

        #define FIND_METHODS_GENERATOR(METHOD_NAME, POS)                                                \
        template <SizeType _Size>                                                                       \
        inline auto METHOD_NAME(const CharT (&str)[_Size], SizeType pos = POS,                          \
                                SizeType n = _Size ? _Size-1 : 0) const {                               \
            return _str_v.METHOD_NAME(str, pos, n);                                                     \
        }                                                                                               \
        template <CharT... _Str>                                                                        \
        inline auto METHOD_NAME(const ConstexprString<CharT, _Str...> str, SizeType pos = POS,          \
                                SizeType n = sizeof...(_Str)) const {                                   \
            return _str_v.METHOD_NAME(str.c_str(), pos, n);                                             \
        }                                                                                               \
        inline auto METHOD_NAME(const StringBase& str, SizeType pos = POS) const {                      \
            return _str_v.METHOD_NAME(str._str_v, pos);                                                 \
        }                                                                                               \
        inline auto METHOD_NAME(CharT c, SizeType pos = POS) const {                                    \
            return _str_v.METHOD_NAME(c, pos);                                                          \
        }

        FIND_METHODS_GENERATOR(find, 0);
        FIND_METHODS_GENERATOR(rfind, npos);
        FIND_METHODS_GENERATOR(find_first_of, 0);
        FIND_METHODS_GENERATOR(find_last_of, npos);
        FIND_METHODS_GENERATOR(find_first_not_of, 0);
        FIND_METHODS_GENERATOR(find_last_not_of, npos);

        inline auto substr(SizeType pos, SizeType n = npos) const {
            return StringBase(_str_v.substr(pos, n));
        }

        inline auto compare(const StringBase& str) const {
            return _str_v.compare(str._str_v);
        }
        template<SizeType _Size>
        inline auto compare(const CharT(&str)[_Size]) const {
            return _str_v.compare(str);
        }
        template<CharT... _Str>
        inline auto compare(const ConstexprString<CharT, _Str...> str) const {
            return _str_v.compare(str.c_str());
        }
        inline auto compare(SizeType pos, SizeType n, const StringBase& str) const {
            return _str_v.compare(pos, n, str._str_v);
        }
        template<SizeType _Size>
        inline auto compare(SizeType pos, SizeType n, const CharT(&str)[_Size]) const {
            return _str_v.compare(pos, n, str);
        }
        template<CharT... _Str>
        inline auto compare(SizeType pos, SizeType n, const ConstexprString<CharT, _Str...> str) const {
            return _str_v.compare(pos, n, str.c_str());
        }
        inline auto compare(SizeType pos1, SizeType n1, const StringBase& str,
                            SizeType pos2, SizeType n2) const {
            return _str_v.compare(pos1, n1, str._str_v, pos2, n2);
        }
        template<SizeType _Size>
        inline auto compare(SizeType pos1, SizeType n1, const CharT(&str)[_Size],
                            SizeType pos2, SizeType n2) const {
            return _str_v.compare(pos1, n1, str, std::basic_string_view(str, _Size));
        }
        template<CharT... _Str>
        inline auto compare(SizeType pos1, SizeType n1, const ConstexprString<CharT, _Str...> str,
                            SizeType pos2, SizeType n2) const {
            return _str_v.compare(pos1, n1, str.str_view(), pos2, n2);
        }

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

        friend std::ostream& operator<< (std::ostream& os, const StringBase& str) {
            os << str.c_str();
            return os;
        }

    protected:
        std::basic_string<CharT> _str_v;
    };

    using String   = StringBase<Char8>;
    using String16 = StringBase<Char16>;
    using String32 = StringBase<Char32>;

} // namespace ftl

#endif //DECAYENGINE_STRING_HPP
