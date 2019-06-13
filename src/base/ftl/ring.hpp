#pragma once

#include <memory>
#include "../baseTypes.hpp"
#include "../assert.hpp"
#include "containers_base.hpp"
#include "string.hpp"

namespace ftl {


    namespace ring_impl {
        template <typename T>
        struct Storage {
            using PtrT = T*;

            bool isFragmented() const { return p_start >= p_end; }

            PtrT distanceGet(SizeT pos) const noexcept {
                auto diff = mem_end - p_start;

                if (isFragmented() && diff <= pos) {
                    pos -= diff;
                    return mem + pos;
                } else
                    return p_start + pos;
            }

            SizeT size = 0;
            SizeT max  = 0;
            PtrT  mem     = nullptr;
            PtrT  mem_end = nullptr;
            PtrT  p_start = nullptr;
            PtrT  p_end   = nullptr;
        };

        template <typename T>
        class RawIterBase : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t, T*, T&> {
            using PtrT = T*;
            using RCT  = std::remove_const_t<T>;
        public:
            RawIterBase(const Storage<RCT>* s, SizeT pos): _s(s) {
                _cur = _s->distanceGet(pos);
            }
            RawIterBase(const Storage<RCT>* s, const PtrT p): _s(s), _cur(p) {}

            PtrT raw_ptr() const { return _cur; }

            auto& operator*()       { return *_cur; }
            auto& operator*() const { return *_cur; }
            PtrT  operator->()      { return _cur; }

        protected:
            void increment() {
                ++_cur;
                if (_s->isFragmented() && _cur == _s->mem_end)
                    _cur = _s->mem;
            }
            void decrement() {
                --_cur;
                if (_s->isFragmented() && _cur < _s->mem)
                    _cur = _s->mem_end - 1;
            }

            void increment(ptrdiff_t val) {
                if (val < 0)
                    decrement(-val);

                _cur += val;
                if (_s->isFragmented() && _cur >= _s->mem_end)
                    _cur = _s->mem + (_cur - _s->mem_end);
            }
            void decrement(ptrdiff_t val) {
                if (val < 0)
                    increment(-val);

                _cur -= val;
                if (_s->isFragmented() && _cur < _s->mem)
                    _cur = _s->mem_end - 1 - (_s->mem - _cur);
            }

        protected:
            const Storage<RCT>* _s;
            PtrT                _cur;
        };

        template <typename T>
        class RawIter : public RawIterBase<T> {
            using inherited = RawIterBase<T>;
            using PtrT = T*;
            using RCT  = std::remove_const_t<T>;

        public:
            RawIter(const Storage<RCT>* s, SizeT pos): inherited(s, pos) {}
            RawIter(const Storage<RCT>* s, PtrT p): inherited(s, p) {}
            RawIter(const RawIter& ri) = default;

            RawIter& operator= (const RawIter& ri) = default;

            bool operator== (const RawIter& ri) { return this->_cur == ri._cur; }
            bool operator!= (const RawIter& ri) { return this->_cur != ri._cur; }

            RawIter& operator++()    { this->increment(); return *this; }
            RawIter& operator--()    { this->decrement(); return *this; }
            RawIter  operator++(int) { RawIter tmp = *this; this->increment(); return tmp; }
            RawIter  operator--(int) { RawIter tmp = *this; this->decrement(); return tmp; }

            RawIter& operator+=(ptrdiff_t val)       { this->increment(val); return *this; }
            RawIter& operator-=(ptrdiff_t val)       { this->decrement(val); return *this; }
            RawIter  operator+ (ptrdiff_t val) const { RawIter tmp = *this; tmp.increment(val); return tmp; }
            RawIter  operator- (ptrdiff_t val) const { RawIter tmp = *this; tmp.decrement(val); return tmp; }
        };

        template <typename T>
        class RawReverseIter : public RawIterBase<T> {
            using inherited = RawIterBase<T>;
            using PtrT = T*;
            using RCT  = std::remove_const_t<T>;

        public:
            RawReverseIter(const Storage<RCT>* s, SizeT pos): inherited(s, pos) {}
            RawReverseIter(const Storage<RCT>* s, const PtrT p): inherited(s, p) {}

            RawReverseIter(const RawReverseIter& ri) = default;

            RawReverseIter& operator= (const RawReverseIter& ri) = default;

            bool operator== (const RawReverseIter& ri) { return this->_cur == ri._cur; }
            bool operator!= (const RawReverseIter& ri) { return this->_cur != ri._cur; }

            RawReverseIter& operator++()    { this->decrement(); return *this; }
            RawReverseIter& operator--()    { this->increment(); return *this; }
            RawReverseIter  operator++(int) { RawIter tmp = *this; this->decrement(); return tmp; }
            RawReverseIter  operator--(int) { RawIter tmp = *this; this->increment(); return tmp; }

            RawReverseIter& operator+=(ptrdiff_t val)       { this->decrement(val); return *this; }
            RawReverseIter& operator-=(ptrdiff_t val)       { this->increment(val); return *this; }
            RawReverseIter  operator+ (ptrdiff_t val) const { RawIter tmp = *this; tmp.decrement(val); return tmp; }
            RawReverseIter  operator- (ptrdiff_t val) const { RawIter tmp = *this; tmp.increment(val); return tmp; }
        };
    }

    template <typename Type, typename AllocatorType = std::allocator<Type>>
    class Ring {
        using SizeType = SizeT;
        using RefType  = Type&;
        using CrefType = const Type&;
        using IterT    = ring_impl::RawIter<Type>;
        using CIterT   = ring_impl::RawIter<const Type>;
        using RIterT   = ring_impl::RawReverseIter<Type>;
        using CRIterT  = ring_impl::RawReverseIter<const Type>;
        using PtrT     = Type*;

    public:
        explicit Ring(const AllocatorType& allocator = AllocatorType()): _allocator(allocator)
        {
            _s.max     = 2;
            _s.mem     = _allocator.allocate(_s.max);
            _s.mem_end = _s.mem + _s.max;
            _s.p_start = _s.mem;
            _s.p_end   = _s.p_start;
        }

        explicit Ring(
            SizeT size,
            const AllocatorType& allocator = AllocatorType()): Ring(allocator)
        {
            reserve(size);
            repeat(size)
                emplace_back(Type());
        }

        Ring(
            SizeT size,
            const Type& fillValue,
            const AllocatorType& allocator = AllocatorType()): Ring(allocator)
        {
            reserve(size);
            repeat(size)
                push_back(fillValue);
        }

        Ring(const Ring& ring,
             const AllocatorType& allocator = AllocatorType()): _allocator(allocator), _s(ring._s)
        {
            _s.mem = _allocator.allocate(ring._s.max);

            auto start_mv = ring._s.p_start - ring._s.mem;
            auto end_mv   = ring._s.p_end   - ring._s.mem;

            std::memcpy(_s.mem, ring._s.mem, _s.max);

            _s.p_start = _s.mem + start_mv;
            _s.p_end   = _s.mem + end_mv;
        }

        Ring(Ring&& ring,
             const AllocatorType& allocator = AllocatorType()) noexcept: _allocator(allocator), _s(std::move(ring._s))
        {
            // Invalidate movable ring
            // Need for valid destruction of movable ring
            ring._s.mem     = nullptr;
            ring._s.mem_end = nullptr;
            ring._s.p_start = nullptr;
            ring._s.p_end   = nullptr;
            ring._s.size = 0;
            ring._s.max  = 0;
        }

        Ring(std::initializer_list<Type> initList, const AllocatorType& allocator = AllocatorType()): Ring(allocator) {
            reserve(initList.size());

            for (auto& i : initList)
                push_back(i);
        }

        ~Ring() {
            for (auto& i : *this)
                i.~Type();
            _allocator.deallocate(_s.mem, 0);
        }


        auto max_size() const noexcept -> SizeT { return _allocator.max_size(); }

        // Iterators
        auto begin  ()       -> IterT   { return  IterT(&_s, _s.p_start); }
        auto end    ()       -> IterT   { return  IterT(&_s, _s.p_end  ); }
        auto begin  () const -> CIterT  { return CIterT(&_s, _s.p_start); }
        auto end    () const -> CIterT  { return CIterT(&_s, _s.p_end  ); }
        auto cbegin () const -> CIterT  { return CIterT(&_s, _s.p_start); }
        auto cend   () const -> CIterT  { return CIterT(&_s, _s.p_end  ); }

        auto rbegin ()       -> RIterT  { return  RIterT(&_s, _s.p_end   - 1); }
        auto rend   ()       -> RIterT  { return  RIterT(&_s, _s.p_start - 1); }
        auto rbegin () const -> CRIterT { return CRIterT(&_s, _s.p_end   - 1); }
        auto rend   () const -> CRIterT { return CRIterT(&_s, _s.p_start - 1); }
        auto crbegin() const -> CRIterT { return CRIterT(&_s, _s.p_end   - 1); }
        auto crend  () const -> CRIterT { return CRIterT(&_s, _s.p_start - 1); }

        // Base functions
        auto back         ()                -> RefType  { ASSERT(_s.size); return *(_s.p_end - 1); }
        auto back         () const          -> CrefType { ASSERT(_s.size); return *(_s.p_end - 1); }
        auto front        () noexcept       -> RefType  { ASSERT(_s.size); return *_s.p_start; }
        auto front        () const noexcept -> CrefType { ASSERT(_s.size); return *_s.p_start; }
        auto empty        () const noexcept -> bool     { return _s.size == 0; }
        auto size         () const noexcept -> SizeT    { return _s.size; }
        auto capacity     () const noexcept -> SizeT    { return _s.max - 1; }

        auto clear() noexcept -> Ring& {
            _s.size    = 0;
            _s.p_start = _s.mem;
            _s.p_end   = _s.p_start;
            return *this;
        }

        auto pop_front() noexcept -> Ring& {
            if (_s.size) {
                --_s.size;
                _incrementStart();
            }
            return *this;
        }

        auto pop_back() noexcept -> Ring& {
            if (_s.size) {
                --_s.size;
                _decrementEnd();
            }
            return *this;
        }

        auto get_allocator() const noexcept -> AllocatorType { return _allocator; }

        auto swap(Ring& ring) noexcept -> Ring& {
            std::swap(_s,ring._s);
            return *this;
        }

        auto reserve(SizeT size) -> Ring& {
            size = size + 1;
            if (size > _s.max) {
                if (_s.size == 0) {
                    _allocator.deallocate(_s.mem, 0);
                    _s.mem = _allocator.allocate(size);
                    _s.mem_end = _s.mem + size;
                    _s.max = size;
                    _s.p_start = _s.mem;
                    _s.p_end   = _s.p_start;
                } else {
                    auto newblock = _allocator.allocate(size);

                    if (_s.isFragmented()) {
                        auto displ = (_s.mem_end - _s.p_start);
                        std::memcpy(newblock, _s.p_start, displ * sizeof(Type));
                        std::memcpy(newblock + displ, _s.mem, (_s.size - displ) * sizeof(Type));
                    } else
                        std::memcpy(newblock, _s.p_start, (_s.p_end - _s.p_start) * sizeof(Type));

                    _allocator.deallocate(_s.mem, 0);

                    _s.mem = newblock;
                    _s.max = size;
                    _s.mem_end = _s.mem + _s.max;
                    _s.p_start = _s.mem;
                    _s.p_end   = _s.mem + _s.size;
                }
            }
            return *this;
        }

        auto resize(SizeT new_size, const Type& fill_value = Type()) -> Ring& {
            auto old_size = size();
            if (new_size > old_size) {
                reserve(new_size);

                IterT last = end() + new_size;
                for (auto i = begin() + old_size; i != last; ++i)
                    new(i.raw_ptr()) Type(fill_value);
            }
            else if (new_size < old_size) {
                auto last = begin() + new_size;

                for (auto i = last; i != end(); ++i)
                    i->~Type();

                _s.p_end = last.raw_ptr();
                _s.size  = new_size;
            }
            return *this;
        }

        auto at(SizeT position)       -> RefType  { return *_s.distanceGet(position); }
        auto at(SizeT position) const -> CrefType { return *_s.distanceGet(position); }

        auto push_back(const Type& value) -> Ring& {
            // Realloc if size == max - 1, because of iterator invalidation (begin == end)
            if (_s.size == _s.max - 1)
                reserve(_s.max * 2);

            ++_s.size;
            _incrementEnd();
            new (_s.p_end - 1) Type(value);
            return *this;
        }

        auto push_front(const Type& value) -> Ring& {
            if (_s.size == _s.max - 1)
                reserve(_s.max * 2);

            ++_s.size;
            _decrementStart();
            new(_s.p_start) Type(value);
            return *this;
        }

        template <typename... Args>
        auto emplace_back(Args&&... args) -> Ring& {
            if (_s.size == _s.max - 1)
                reserve(_s.max * 2);

            ++_s.size;
            _incrementEnd();
            new(_s.p_end - 1) Type(args...);
            return *this;
        }

        template <typename... Args>
        auto emplace_front(Args&&... args) -> Ring& {
            if (_s.size == _s.max - 1)
                reserve(_s.max * 2);

            ++_s.size;
            _decrementStart();
            new(_s.p_start) Type(args...);
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
        auto map(Function callback) const -> Ring<ttr::return_type_of<Function>> {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );
            auto mapped = Ring<ttr::return_type_of<Function>>();
            mapped.reserve(size());

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto &item : *this)
                    mapped.emplace_back(callback(item));
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto &item : *this)
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
        auto filter(Function callback) const -> Ring {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );

            auto filtered = Ring();

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto& item : *this)
                    if (callback(item))
                        filtered.emplace_back(item);
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto& item : *this)
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
        auto foreach(Function callback) -> Ring& {
            static_assert(
                    ttr::args_count<Function> == 1 ||
                    ttr::args_count<Function> == 2,
                    "Callback has wrong number of arguments"
            );

            if constexpr (ttr::args_count<Function> == 1) {
                for (auto &item : *this)
                    callback(item);
            } else if constexpr (ttr::args_count<Function> == 2) {
                SizeT i = 0;
                for (auto &item : *this)
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
        auto sort(Function callback) -> Ring& {
            std::sort(begin(), end(), callback);
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

        // Operators
        friend std::ostream& operator<< (std::ostream& os, const Ring& ring) { ring.print(os); return os; }

        auto operator[] (SizeT position) noexcept -> RefType {
            ASSERT(position < size());
            return *_s.distanceGet(position);
        }
        auto operator[] (SizeT position) const noexcept -> CrefType {
            ASSERT(position < size());
            return *_s.distanceGet[position];
        }

        bool operator== (const Ring& r) const {
            return std::equal(r.cbegin(), r.cend(), cbegin());
        }

    private:
        void _incrementStart() {
            ++_s.p_start;
            if (_s.p_start == _s.mem_end)
                _s.p_start = _s.mem;
        }
        void _decrementStart() {
            --_s.p_start;
            if (_s.p_start < _s.mem)
                _s.p_start = _s.mem_end - 1;
        }

        void _incrementEnd() {
            ++_s.p_end;
            if (_s.p_end > _s.mem_end)
                _s.p_end = _s.mem + 1;
        }
        void _decrementEnd() {
            --_s.p_end;
            if (_s.p_end == _s.mem)
                _s.p_end = _s.mem_end;
        }


    private:
        AllocatorType            _allocator;
        ring_impl::Storage<Type> _s;

    };
} // namespace ftl