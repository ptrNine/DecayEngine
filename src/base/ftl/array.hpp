#ifndef UNTITLED6_ARRAY_HPP
#define UNTITLED6_ARRAY_HPP

#include <array>
#include <iostream>
#include <sstream>
#include <functional>
#include "containers_base.hpp"
#include "string.hpp"

namespace ftl {
    template <typename Type, SizeT _Size>
    class Array {
    public:
        using ValType        = Type;
        using Iterator       = typename std::array<Type, _Size>::iterator;
        using ConstIterator  = typename std::array<Type, _Size>::const_iterator;
        using RIterator      = typename std::array<Type, _Size>::reverse_iterator;
        using ConstRIterator = typename std::array<Type, _Size>::const_reverse_iterator;

        static constexpr SizeT ArraySize = _Size;

    protected:
        template <typename Function, SizeT pos, SizeT count>
        inline static constexpr auto
        _count_elements(Function callback, const Array& array) noexcept ->
        typename std::enable_if<_Size == pos, SizeT>::type {
            return count;
        }

        template <typename Function, SizeT pos = 0, SizeT count = 0>
        inline static constexpr auto
        _count_elements(Function callback, const Array& array) noexcept ->
        typename std::enable_if<_Size != pos, SizeT>::type {
            return callback(std::get<pos>(array._stl_array)) ?
                   _count_elements<Function, pos + 1, count + 1>(callback, array) :
                   _count_elements<Function, pos + 1, count>(callback, array);
        }

        template <typename Function, SizeT pos>
        inline static constexpr void
        _foreach_iter(Function callback, Array& array) noexcept {
            callback(std::get<pos>(array._stl_array));
            if constexpr (pos + 1 < _Size)
                _foreach_iter<Function, pos + 1>(callback, array);
        }

    public:

        // No arguments, noexcept
        constexpr auto size    () const noexcept -> SizeT       { return _stl_array.size(); }
        constexpr auto max_size() const noexcept -> SizeT       { return _stl_array.max_size(); }
        constexpr auto empty   () const noexcept -> bool        { return _stl_array.empty(); }
        constexpr auto back    () noexcept       -> Type&       { return _stl_array.back(); }
        constexpr auto back    () const noexcept -> const Type& { return _stl_array.back(); }
        constexpr auto front   () noexcept       -> Type&       { return _stl_array.front(); }
        constexpr auto front   () const noexcept -> const Type& { return _stl_array.front(); }
        constexpr auto data    () noexcept       -> Type*       { return _stl_array.data(); }
        constexpr auto data    () const noexcept -> const Type* { return _stl_array.data(); }

        // Iterators
        constexpr auto begin   () noexcept       -> Iterator       { return _stl_array.begin(); }
        constexpr auto end     () noexcept       -> Iterator       { return _stl_array.end(); }
        constexpr auto begin   () const noexcept -> ConstIterator  { return _stl_array.begin(); }
        constexpr auto end     () const noexcept -> ConstIterator  { return _stl_array.end(); }
        constexpr auto cbegin  () const noexcept -> ConstIterator  { return _stl_array.cbegin(); }
        constexpr auto cend    () const noexcept -> ConstIterator  { return _stl_array.cend(); }
        constexpr auto rbegin  () noexcept       -> RIterator      { return _stl_array.rbegin(); }
        constexpr auto rend    () noexcept       -> RIterator      { return _stl_array.rend(); }
        constexpr auto rbegin  () const noexcept -> ConstRIterator { return _stl_array.rbegin(); }
        constexpr auto rend    () const noexcept -> ConstRIterator { return _stl_array.rend(); }
        constexpr auto crbegin () const noexcept -> ConstRIterator { return _stl_array.crbegin(); }
        constexpr auto crend   () const noexcept -> ConstRIterator { return _stl_array.crend(); }

        constexpr auto at (SizeT position)       -> Type&       { return _stl_array.at(position); }
        constexpr auto at (SizeT position) const -> const Type& { return _stl_array.at(position); }

        auto fill (const Type& value) -> Array& { _stl_array.fill(std::cref(value)); return *this; }
        auto swap (Array& array)      -> Array& { _stl_array.swap(array._stl_array); return *this; }

        // Operators
        constexpr auto operator[] (SizeT position) noexcept       -> Type&       { return _stl_array[position]; }
        constexpr auto operator[] (SizeT position) const noexcept -> const Type& { return _stl_array[position]; }

        friend std::ostream& operator<< (std::ostream& os, const Array& array) { array.print(std::ref(os)); return std::ref(os); }



        // New methods

        // Compile-time methods
        template <typename Function>
        inline constexpr auto count_elements_if(Function callback) const -> SizeT {
            return _count_elements(callback, *this);
        }

        template <typename Function>
        constexpr auto cmap(Function callback) const {
            auto array = Array<typename ftl::function_traits<Function>::return_type, _Size>();
            return _map_iter<Function>(callback, array);
        }


        // Runtime
        /**
         *
         * @tparam RedTp - return type of callback and type of its first argument
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(RedTp res, Type item) -> RedTp
         * @param RedTp res - accumulated value
         * @param Type item - one item from vector
         * @param[in] init - initial value with RedTp type
         * @return accumulated value with RedTp type
         */
        template <typename RedTp, typename Function>
        auto reduce(Function callback, RedTp init) const {
            return ftl::_iter_reduce(cbegin(), cend(), callback, init);
        }

        /**
         *
         * @tparam RedTp - return type of callback and type of its first argument
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(RedTp res, Type item) -> RedTp
         * @param RedTp res - accumulated value
         * @param Type item - one item from vector
         * @return accumulated value with RedTp type
         */
        template <typename Function>
        auto reduce(Function callback) const {
            return ftl::_iter_reduce(cbegin(), cend(), callback);
        }
        /**
         *
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(Type item) -> Type
         * @param Type item - one item from vector
         * @return Vector with mapped values
         */
        template <typename Function>
        auto map(Function callback) const -> Array<ttr::return_type_of<Function>, _Size> {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );
            auto mapped = Array<ttr::return_type_of<Function>, _Size>();

            if constexpr (ttr::args_count<Function> == 1) {
                for (SizeT i = 0; i < _Size; ++i)
                    mapped.at(i) = callback(at(i));
            } else if constexpr (ttr::args_count<Function> == 2) {
                for (SizeT i = 0; i < _Size; ++i)
                    mapped.at(i) = callback(at(i), i);
            }
            return std::move(mapped);
        }
        /**
         *
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(Type item) -> void
         * @param Type item - one item from vector
         * @return this Vector
         */
        template <typename Function>
        auto foreach(Function callback) -> Array& {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto &item : _stl_array)
                    callback(item);
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto &item : _stl_array)
                    callback(item, i++);
            }

            return *this;
        }
        /**
         *
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(Type l, Type r) -> bool
         * @param Type l - one item from vector
         * @param Type r - next item from vector
         * @return this Vector
         */
        template <typename Function>
        auto sort(Function callback) -> Array& {
            std::sort(_stl_array.begin(), _stl_array.end(), callback);
            return *this;
        }
        auto to_string() const -> ftl::String {
            auto sstream = std::stringstream();
            print(sstream);
            return sstream.str();
        }

        void print(std::ostream& os = std::cout) const {
            switch (_Size) {
                case 0: os << "{}"; return;
                case 1: os << "{ " << front() << " }"; return;
                default:
                    os << "{ " << front();
                    for (auto it = cbegin() + 1; it != cend(); ++it)
                        os << ", " << *it;
                    os << " }";
            }
        }

        U64 hash() const { return XXH64(_stl_array.data(), _stl_array.size() * sizeof(Type), 0); }

    protected:
        // constexpr cmap realization
        template <typename Function, typename NewType, SizeT pos>
        constexpr auto _map_iter(Function callback, Array<NewType, _Size>& newArray) const
        -> typename std::enable_if_t<pos == _Size, Array<NewType, _Size>>
        {
            return newArray;
        }

        template <typename Function, typename NewType, SizeT pos = 0>
        constexpr auto _map_iter(Function callback, Array<NewType, _Size>& newArray) const
        -> typename std::enable_if_t<
                pos != _Size &&
                ftl::function_traits<Function>::arity == 1,
                Array<NewType, _Size>>
        {
            std::get<pos>(newArray._stl_array) = callback(std::get<pos>(_stl_array));
            return _map_iter<Function, NewType, pos + 1>(callback, newArray);
        }

        template <typename Function, typename NewType, SizeT pos = 0>
        constexpr auto _map_iter(Function callback, Array<NewType, _Size>& newArray) const
        -> typename std::enable_if_t<
                pos != _Size &&
                ftl::function_traits<Function>::arity == 2,
                Array<NewType, _Size>>
        {
            std::get<pos>(newArray._stl_array) = callback(std::get<pos>(_stl_array), pos);
            return _map_iter<Function, NewType, pos + 1>(callback, newArray);
        }

        template <typename Function, typename NewType, SizeT pos>
        constexpr auto _reduce_iter(Function callback, NewType result) const
        -> typename std::enable_if_t<pos == _Size, NewType> {
            return result;
        }

        template <typename Function, typename NewType, SizeT pos>
        constexpr auto _reduce_iter(Function callback, NewType result) const
        -> typename std::enable_if_t<pos != 0 && pos != _Size, NewType> {
            return _reduce_iter<Function, NewType, pos + 1> (
                    callback,
                    callback(result, std::get<pos + 1>(_stl_array)));
        }

        template <typename Function, typename NewType, SizeT pos = 0>
        constexpr auto _reduce_iter(Function callback) const
        -> typename std::enable_if_t<pos == 0, NewType> {
            return _reduce_iter<Function, NewType, pos + 1> (
                        callback,
                        callback(std::get<pos>    (_stl_array),
                                 std::get<pos + 1>(_stl_array)));
        }


    public:
        std::array<Type, _Size> _stl_array;
    };

    template<typename Type, typename... ValType>
    Array(Type, ValType...)
    -> Array<std::enable_if_t<(std::is_same_v<Type, ValType> && ...), Type>, 1 + sizeof...(ValType)>;

    template<typename Type, SizeT _size>
    inline auto operator==(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return std::equal(one.begin(), one.end(), two.begin());
    }
    template<typename Type, SizeT _size>
    inline auto operator!=(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return !(one == two);
    }
    template<typename Type, SizeT _size>
    inline auto operator<(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return std::lexicographical_compare(one.begin(), one.end(), two.begin(), two.end());
    }
    template<typename Type, SizeT _size>
    inline auto operator>(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return two < one;
    }
    template<typename Type, SizeT _size>
    inline auto operator>=(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return !(one < two);
    }
    template<typename Type, SizeT _size>
    inline auto operator<=(const Array<Type, _size>& one, const Array<Type, _size>& two) -> bool {
        return !(one > two);
    }

    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(ftl::Array<Type, _size>& array) noexcept -> Type& {
        return std::get<_pos>(array._stl_array);
    }
    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(const ftl::Array<Type, _size>& array) noexcept -> const Type& {
        return std::get<_pos>(array._stl_array);
    }
    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(ftl::Array<Type, _size>&& array) noexcept -> Type&& {
        return std::move(std::get<_pos>(array._stl_array));
    }
}

namespace std {
    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(ftl::Array<Type, _size>& array) noexcept -> Type& {
        return std::get<_pos>(array._stl_array);
    }
    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(const ftl::Array<Type, _size>& array) noexcept -> const Type& {
        return std::get<_pos>(array._stl_array);
    }
    template<SizeT _pos, typename Type, SizeT _size>
    constexpr auto get(ftl::Array<Type, _size>&& array) noexcept -> Type&& {
        return std::move(std::get<_pos>(array._stl_array));
    }
}

namespace details {
    template<typename Type, SizeT _size, typename Function, typename... Types>
    constexpr auto map(Function callback, Types... args) -> ftl::Array<Type, _size> {
        std::tuple<Types...> result((callback(args))...);
        return result;
    }
}

namespace std {
    template<typename Type, SizeT _size>
    struct tuple_size<ftl::Array<Type, _size>> : public integral_constant<SizeT, _size> {};

    template<SizeT _pos, typename Type, SizeT _size>
    struct tuple_element<_pos, ftl::Array<Type, _size>> : tuple_element<_pos, std::array<Type, _size>> {};
}

// fmt format
template <typename Type, SizeT _Size>
struct fmt::formatter<ftl::Array<Type, _Size>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ftl::Array<Type, _Size>& array, FormatContext& ctx) {
        return format_to(ctx.out(), "{}", array.to_string());
    }
};

// std chash
template <typename Type, SizeT _Size>
struct std::hash<ftl::Array<Type, _Size>> {
    U64 operator()(const ftl::Array<Type, _Size>& vec) const {
        return vec.hash();
    }
};
#endif //UNTITLED6_ARRAY_HPP
