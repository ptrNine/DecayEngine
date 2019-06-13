#ifndef DECAYENGINE_VECTOR_HPP
#define DECAYENGINE_VECTOR_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <numeric>
#include "containers_base.hpp"
#include "string.hpp"

namespace ftl {
    template<typename Type>
    class Vector {
        using iterator         = typename std::vector<Type>::iterator;
        using const_iterator   = typename std::vector<Type>::const_iterator;
        using r_iterator       = typename std::vector<Type>::reverse_iterator;
        using const_r_iterator = typename std::vector<Type>::const_reverse_iterator;
        using allocator_type   = typename std::vector<Type>::allocator_type;
        using RefType          = typename std::vector<Type>::reference;
        using C_RefType        = typename std::vector<Type>::const_reference;

    public:
        using ValType = Type;

        ~Vector () noexcept = default;

        // Constructors
        Vector  () noexcept : _stl_vector() {}

        explicit
        Vector (const allocator_type& allocator) noexcept : _stl_vector(std::cref(allocator)){}

        explicit
        Vector (SizeT size, const allocator_type& allocator = allocator_type())
            : _stl_vector(size, std::cref(allocator)){}

        Vector (SizeT size, const Type& fillValue, const allocator_type& allocator = allocator_type())
            : _stl_vector(size, std::cref(fillValue), std::cref(allocator)){}

        Vector (const Vector& vector, const allocator_type& allocator)
            : _stl_vector(std::cref(vector._stl_vector), std::cref(allocator)){}

        Vector (Vector&& vector, const allocator_type& allocator) noexcept
            : _stl_vector(std::move(vector._stl_vector), std::cref(allocator)){}

        Vector (const Vector& vector)                 : _stl_vector(std::cref(vector._stl_vector)){}

        Vector (Vector&& vector) noexcept             : _stl_vector(std::move(vector._stl_vector)){}

        Vector (std::initializer_list<Type> initList, const allocator_type& allocator = allocator_type())
            : _stl_vector(initList, std::cref(allocator)){}

        template<typename InpIter, typename = std::_RequireInputIter<InpIter>>
        Vector (InpIter first, InpIter last, const allocator_type& allocator = allocator_type())
            : _stl_vector(first, last, std::cref(allocator)){}




        // Base functions

        // No arguments, noexcept
        auto back         () noexcept           -> RefType        { return _stl_vector.back(); }
        auto back         () const noexcept     -> C_RefType      { return _stl_vector.back(); }
        auto front        () noexcept           -> RefType        { return _stl_vector.front(); }
        auto front        () const noexcept     -> C_RefType      { return _stl_vector.front(); }
        auto data         () noexcept           -> Type*          { return _stl_vector.data(); }
        auto data         () const noexcept     -> const Type*    { return _stl_vector.data(); }
        auto empty        () const noexcept     -> bool           { return _stl_vector.empty(); }
        auto size         () const noexcept     -> SizeT          { return _stl_vector.size(); }
        auto max_size     () const noexcept     -> SizeT          { return _stl_vector.max_size(); }
        auto capacity     () const noexcept     -> SizeT          { return _stl_vector.capacity(); }
        auto clear        () noexcept           -> Vector&        { _stl_vector.clear();         return *this; }
        auto flip         () noexcept           -> Vector&        { _stl_vector.flip();          return *this; }
        auto shrink_to_fit()                    -> Vector&        { _stl_vector.shrink_to_fit(); return *this; }
        auto pop_back     () noexcept           -> Vector&        { _stl_vector.pop_back();      return *this; }
        auto get_allocator() const noexcept     -> allocator_type { return _stl_vector.get_allocator(); }

        // Iterators
        auto begin   () noexcept       -> iterator         { return _stl_vector.begin(); }
        auto end     () noexcept       -> iterator         { return _stl_vector.end(); }
        auto begin   () const noexcept -> const_iterator   { return _stl_vector.begin(); }
        auto end     () const noexcept -> const_iterator   { return _stl_vector.end(); }
        auto cbegin  () const noexcept -> const_iterator   { return _stl_vector.cbegin(); }
        auto cend    () const noexcept -> const_iterator   { return _stl_vector.cend(); }
        auto rbegin  () noexcept       -> r_iterator       { return _stl_vector.rbegin(); }
        auto rend    () noexcept       -> r_iterator       { return _stl_vector.rend(); }
        auto rbegin  () const noexcept -> const_r_iterator { return _stl_vector.rbegin(); }
        auto rend    () const noexcept -> const_r_iterator { return _stl_vector.rend(); }
        auto crbegin () const noexcept -> const_r_iterator { return _stl_vector.crbegin(); }
        auto crend   () const noexcept -> const_r_iterator { return _stl_vector.crend(); }

        auto swap     (Vector& vector) noexcept -> Vector&  { _stl_vector.swap(vector); return *this; }
        auto at       (SizeT position)         -> RefType   { return _stl_vector.at(position); }
        auto at       (SizeT position) const   -> C_RefType { return _stl_vector.at(position); }
        auto reserve  (SizeT size)             -> Vector&   { _stl_vector.reserve(size); return *this; }
        auto resize   (SizeT newSize)          -> Vector&   { _stl_vector.resize(newSize); return *this; }
        auto push_back(const Type& value)       -> Vector&  { _stl_vector.push_back(value); return *this; }

        auto resize (SizeT newSize, const Type& fillValue) -> Vector& {
            _stl_vector.resize(newSize, fillValue);
            return *this;
        }

        auto push_back(Type&& value) -> Vector& {
            _stl_vector.emplace_back(std::move(value));
            return *this;
        }

        template<typename... Arguments>
        auto emplace_back(Arguments&&... args) {
            return _stl_vector.emplace_back(args...);
        }

        template<typename... Arguments>
        auto emplace(const_iterator position, Arguments&&... args) -> iterator {
            return _stl_vector.emplace(args...);
        }

        auto insert(const_iterator position, const Type& value) -> iterator {
            return _stl_vector.insert(position, std::cref(value));
        }
        auto insert(const_iterator position, Type&& value) -> iterator {
            return _stl_vector.insert(position, std::move(value));
        }
        auto insert(const_iterator position, std::initializer_list<Type> initList) -> iterator {
            return _stl_vector.insert(position, initList);
        }
        auto insert(const_iterator position, SizeT n, const Type& value) -> iterator {
            return _stl_vector.insert(position, n, std::cref(value));
        }
        template<typename InpIter, typename = std::_RequireInputIter<InpIter>>
        auto insert(const_iterator position, InpIter first, InpIter last) -> iterator {
            return _stl_vector.insert(position, first, last);
        }

        auto erase(const_iterator position) -> iterator {
            return _stl_vector.erase(position);
        }
        auto erase(const_iterator first, const_iterator last) -> iterator {
            return _stl_vector.erase(first, last);
        }

        auto assign(SizeT n, const Type& value) -> Vector& {
            _stl_vector.assign(n, std::cref(value));
            return *this;
        }
        template<typename InpIter, typename = std::_RequireInputIter<InpIter>>
        auto assign(InpIter first, InpIter last) -> Vector& {
            _stl_vector.assign(first, last);
            return *this;
        }
        auto assign(std::initializer_list<Type> initList) -> Vector& {
            _stl_vector.assign(initList);
            return *this;
        }


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
        auto map(Function callback) const -> Vector<ttr::return_type_of<Function>> {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );
            auto mapped = Vector<ttr::return_type_of<Function>>();
            mapped.reserve(size());

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto &item : _stl_vector)
                    mapped.emplace_back(callback(item));
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto &item : _stl_vector)
                    mapped.emplace_back(callback(item, i++));
            }
            return std::move(mapped);
        }
        /**
         *
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(Type item) -> bool
         * @param Type item - one item from vector
         * @return Vector with filtered values
         */
        template <typename Function>
        auto filter(Function callback) const -> Vector {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );

            auto filtered = Vector();

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto& item : _stl_vector)
                    if (callback(item))
                        filtered.emplace_back(item);
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto& item : _stl_vector)
                    if (callback(item, i++))
                        filtered.emplace_back(item);
            }

            return std::move(filtered);
        }
        /**
         *
         * @tparam Function - type of callback
         * @param[in] callback - function like auto f(Type item) -> void
         * @param Type item - one item from vector
         * @return this Vector
         */
        template <typename Function>
        auto foreach(Function callback) -> Vector& {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto &item : _stl_vector)
                    callback(item);
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto &item : _stl_vector)
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
        auto sort(Function callback) -> Vector& {
            std::sort(_stl_vector.begin(), _stl_vector.end(), callback);
            return *this;
        }

        auto to_string() const -> ftl::String {
            auto sstream = std::stringstream();
            print(sstream);
            return sstream.str();
        }

        void print(std::ostream& os = std::cout) const {
            ftl::_iter_print(cbegin(), cend(), size(), os);
        }

        U64 hash() const { return XXH64(_stl_vector.data(), _stl_vector.size() * sizeof(Type), 0); }


        // Operators
        auto operator= (const Vector& vector) -> Vector& {
            _stl_vector.operator=(std::cref(vector._stl_vector));
            return *this;
        }
        auto operator= (Vector&& vector) noexcept -> Vector& {
            _stl_vector.operator=(std::move(vector._stl_vector));
            return *this;
        }
        auto operator= (std::initializer_list<Type> initList) -> Vector& {
            _stl_vector.operator=(std::move(initList));
            return *this;
        }

        auto operator[] (SizeT position) noexcept -> RefType {
            return _stl_vector[position];
        }
        auto operator[] (SizeT position) const noexcept -> C_RefType {
            return _stl_vector[position];
        }

        bool operator== (const Vector& vector) const {
            if (size() != vector.size())
                return false;

            for (SizeT i = 0; i < size(); ++i) {
                if (operator[](i) != vector[i])
                    return false;
            }
            return true;
        }

        friend std::ostream& operator<< (std::ostream& os, const Vector& vector) { vector.print(os); return os; }

    protected:
        std::vector<Type> _stl_vector;
    };
}

// fmt format
template <typename Type>
struct fmt::formatter<ftl::Vector<Type>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ftl::Vector<Type>& vec, FormatContext& ctx) {
        return format_to(ctx.out(), "{}", vec.to_string());
    }
};

// std hash
template <typename Type>
struct std::hash<ftl::Vector<Type>> {
    U64 operator()(const ftl::Vector<Type>& vec) const {
        return vec.hash();
    }
};


#endif //DECAYENGINE_VECTOR_HPP
