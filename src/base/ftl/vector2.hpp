#ifndef UIBUILDER_VECTOR2_HPP
#define UIBUILDER_VECTOR2_HPP

#include "../math.hpp"
#include "../concepts.hpp"
#include "../baseTypes.hpp"
#include <type_traits>
#include <utility>

#define ICA inline constexpr auto

#ifndef VECTOR_FISR_ITERS_COUNT
#define VECTOR_FISR_ITERS_COUNT 3
#endif

namespace ftl {

    template<typename Type>
    class Vector2 {
        static_assert(concepts::numbers<Type>, "Template type must be number");

    public:
        Vector2() : _x(0), _y(0) {}

        Vector2(Type x, Type y) : _x(x), _y(y) {}

        explicit
        Vector2(Type val) : _x(val), _y(val) {}


    public:
        ICA& set(Type x, Type y) noexcept {
            _x = x;
            _y = y;
            return *this;
        }

        ICA& x() noexcept { return _x; }
        ICA& y() noexcept { return _y; }

        const ICA& x() const noexcept { return _x; }
        const ICA& y() const noexcept { return _y; }

        // Same to '+', '-' ... operators
        ICA makeAdd(const Vector2& r) const {
            return Vector2(_x + r._x, _y + r._y);
        }

        ICA makeSub(const Vector2& r) const {
            return Vector2(_x - r._x, _y - r._y);
        }

        ICA makeScalarAdd(const Type& val) const {
            return Vector2(_x + val, _y + val);
        }

        ICA makeScalarSub(const Type& val) const {
            return Vector2(_x - val, _y - val);
        }

        ICA makeScalarMul(const Type& val) const {
            return Vector2(_x * val, _y * val);
        }

        ICA makeScalarDiv(const Type& val) const {
            return Vector2(_x / val, _y / val);
        }

        // Same to '+=', '-=', ... operators
        ICA& add(const Vector2& r) {
            _x += r._x;
            _y += r._y;
            return *this;
        }

        ICA& sub(const Vector2& r) {
            _x -= r._x;
            _y -= r._y;
            return *this;
        }

        ICA& scalarAdd(const Type& val) {
            _x += val;
            _y += val;
            return *this;
        }

        ICA& scalarSub(const Type& val) {
            _x -= val;
            _y -= val;
            return *this;
        }

        ICA& scalarMul(const Type& val) {
            _x *= val;
            _y *= val;
            return *this;
        }

        ICA& scalarDiv(const Type& val) {
            _x /= val;
            _y /= val;
            return *this;
        }

        ICA divProduct(const Vector2& r) const { return _x * r._x + _y * r._y; }

        ICA magnitude2() { return _x * _x + _y * _y; }


        // Operators

        // Equality
        ICA operator==(const Vector2& r) const { return _x == r._x && _y == r._y; }
        ICA operator!=(const Vector2& r) const { return !(*this == r); }

        // Vectors
        ICA  operator+(const Vector2& r) const { return makeAdd(r); }
        ICA  operator-(const Vector2& r) const { return makeSub(r); }

        ICA& operator+=(const Vector2& r) { return add(r); }
        ICA& operator-=(const Vector2& r) { return sub(r); }

        // Scalars
        ICA  operator+(const Type& val) const { return makeScalarAdd(val); }
        ICA  operator-(const Type& val) const { return makeScalarSub(val); }
        ICA  operator*(const Type& val) const { return makeScalarMul(val); }
        ICA  operator/(const Type& val) const { return makeScalarDiv(val); }

        ICA& operator+=(const Type& val) { return scalarAdd(val); }
        ICA& operator-=(const Type& val) { return scalarSub(val); }
        ICA& operator*=(const Type& val) { return scalarMul(val); }
        ICA& operator/=(const Type& val) { return scalarDiv(val); }

        template<SizeT _pos>
        friend constexpr auto get(Vector2<Type> &v) noexcept -> Type & {
            if constexpr (_pos == 0) return v._x;
            return v._y;
        }

        template<SizeT _pos>
        friend constexpr auto get(const Vector2<Type> &v) noexcept -> const Type & {
            if constexpr (_pos == 0) return v._x;
            return v._y;
        }

        template<SizeT _pos>
        friend constexpr auto get(Vector2<Type> &&v) noexcept -> Type && {
            if constexpr (_pos == 0) return std::move(v._x);
            return std::move(v._y);
        }

    protected:
        Type _x, _y;
    };
} // namespace ftl

namespace std {

    template<typename Type>
    struct tuple_size<ftl::Vector2<Type>> : public integral_constant<SizeT, 2> {
    };

    template<SizeT _pos, typename Type>
    struct tuple_element<_pos, ftl::Vector2<Type>> {
        using type = Type;
    };
}

namespace ftl {
    template<typename Type>
    class Vector2Flt : public Vector2<Type> {
        using inherited = Vector2<Type>;
    public:
        Vector2Flt() : inherited() {}

        Vector2Flt(Type x, Type y) : inherited(x, y) {}

        explicit
        Vector2Flt(Type val) : inherited(val) {}


        template<std::size_t _steps = 1>
        ICA fastInvMagnitude() { return math::fast_inv_sqrt<_steps>(this->magnitude2()); }

        template<std::size_t _steps = 1>
        ICA fastMagnitude() { return 1 / fastInvMagnitude<_steps>(); }

        template<std::size_t _steps = 1>
        ICA& fastNormalize() { return ((*this) *= fastInvMagnitude<_steps>()); }

        ICA magnitude() { return 1 / fastInvMagnitude<VECTOR_FISR_ITERS_COUNT>(); }

        ICA& normalize() { return ((*this) *= fastInvMagnitude<VECTOR_FISR_ITERS_COUNT>()); }
    };

} // namespace ftl


namespace std {
    template <typename Type>
    struct tuple_size<ftl::Vector2Flt<Type>> : public integral_constant<std::size_t, 2> {};

    template <size_t _pos, typename Type>
    struct tuple_element<_pos, ftl::Vector2Flt<Type>> { using type = Type; };
}


namespace ftl {
    using Vector2u   = Vector2<unsigned long>;
    using Vector2i   = Vector2<long>;

    using Vector2u8  = Vector2<U8>;
    using Vector2u16 = Vector2<U16>;
    using Vector2u32 = Vector2<U32>;
    using Vector2u64 = Vector2<U64>;
    using Vector2s8  = Vector2<S8>;
    using Vector2s16 = Vector2<S16>;
    using Vector2s32 = Vector2<S32>;
    using Vector2s64 = Vector2<S64>;

    using Vector2f   = Vector2Flt<Float32>;
    using Vector2d   = Vector2Flt<Float64>;
    using Vector2f32 = Vector2Flt<Float32>;
    using Vector2f64 = Vector2Flt<Float64>;

    template <typename Type, std::enable_if_t<concepts::integers<Type>>...>
    ICA Vector2T(Type x, Type y) {
        return Vector2<Type>(x, y);
    }
    template <typename Type, std::enable_if_t<concepts::floats<Type>>...>
    ICA Vector2T(Type x, Type y) {
        return Vector2Flt<Type>(x, y);
    }

    template <typename OutType, typename InType, std::enable_if_t<concepts::integers<InType>>...>
    ICA Vector2Convert(const Vector2<InType>& vec) {
        return Vector2T<OutType>(vec.x(), vec.y());
    }
    template <typename OutType, typename InType, std::enable_if_t<concepts::floats<InType>>...>
    ICA Vector2Convert(const Vector2Flt<InType>& vec) {
        return Vector2T<OutType>(vec.x(), vec.y());
    }

} // namespace ftl

#undef ICA // inline constexpr auto

#endif //UIBUILDER_VECTOR2_HPP
