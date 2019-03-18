#ifndef UIBUILDER_VECTOR2_HPP
#define UIBUILDER_VECTOR2_HPP

#include "math.hpp"
#include <type_traits>
#include <utility>

#define ICA inline constexpr auto

template <typename T>
using NumberCheck = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>;

template <typename T>
using FloatCheck = std::enable_if_t<std::is_floating_point_v<T>>;


template <typename Type, typename = NumberCheck<Type>>
class Vector2 {
public:
    Vector2()               : _x(0),   _y(0)   {}
    Vector2(Type x, Type y) : _x(x),   _y(y)   {}
    explicit
    Vector2(Type val)       : _x(val), _y(val) {}


public:
    ICA x() noexcept -> Type&              { return _x; }
    ICA y() noexcept -> Type&              { return _y; }
    ICA x() const noexcept -> const Type&  { return _x; }
    ICA y() const noexcept -> const Type&  { return _y; }


    // Same to '+=', '-=', ... operators
    ICA& add       (const Vector2& r) { _x += r._x; _y += r._y; return *this; }
    ICA& sub       (const Vector2& r) { _x -= r._x; _y -= r._y; return *this; }
    ICA& scalarAdd (const Type& val)  { _x += val;  _y += val;  return *this; }
    ICA& scalarSub (const Type& val)  { _x -= val;  _y -= val;  return *this; }
    ICA& scalarMul (const Type& val)  { _x *= val;  _y *= val;  return *this; }
    ICA& scalarDiv (const Type& val)  { _x /= val;  _y /= val;  return *this; }


    ICA divProduct(const Vector2& r) const { return _x * r._x + _y * r._y; }

    ICA magnitude2() { return _x * _x + _y * _y; }


    // Operators

    // Equality
    ICA operator== (const Vector2& r)  { return _x == r._x && _y == r._y; }
    ICA operator!= (const Vector2& r)  { return !(*this == r); }

    // Vectors
    ICA  operator+  (const Vector2& r) { return Vector2(add(r)); }
    ICA  operator-  (const Vector2& r) { return Vector2(sub(r)); }
    ICA& operator+= (const Vector2& r) { return add(r); }
    ICA& operator-= (const Vector2& r) { return sub(r); }

    // Scalars
    ICA  operator+  (const Type& val)  { return Vector2(scalarAdd(val)); }
    ICA  operator-  (const Type& val)  { return Vector2(scalarSub(val)); }
    ICA  operator*  (const Type& val)  { return Vector2(scalarMul(val)); }
    ICA  operator/  (const Type& val)  { return Vector2(scalarDiv(val)); }
    ICA& operator+= (const Type& val)  { return scalarAdd(val); }
    ICA& operator-= (const Type& val)  { return scalarSub(val); }
    ICA& operator*= (const Type& val)  { return scalarMul(val); }
    ICA& operator/= (const Type& val)  { return scalarDiv(val); }

    template<std::size_t _pos>
    friend constexpr auto get(Vector2<Type>& v) noexcept -> Type& {
        if constexpr (_pos == 0) return v._x; return v._y;
    }

    template<std::size_t _pos>
    friend constexpr auto get(const Vector2<Type>& v) noexcept -> const Type& {
        if constexpr (_pos == 0) return v._x; return v._y;
    }

    template<std::size_t _pos>
    friend constexpr auto get(Vector2<Type>&& v) noexcept -> Type&& {
        if constexpr (_pos == 0) return v._x; return v._y;
    }

protected:
    Type _x, _y;
};

namespace std {

    template <typename Type>
    struct tuple_size<Vector2<Type>> : public integral_constant<std::size_t, 2> {};

    template <size_t _pos, typename Type>
    struct tuple_element<_pos, Vector2<Type>> { using type = Type; };
}


template <typename Type, typename = FloatCheck<Type>>
class Vector2Flt : public Vector2<Type> {
    using inherited = Vector2<Type, FloatCheck<Type>>;
public:
    Vector2Flt()               : inherited()     {}
    Vector2Flt(Type x, Type y) : inherited(x, y) {}
    explicit
    Vector2Flt(Type val)       : inherited(val)  {}


    template <std::size_t _steps = 1>
    ICA fastInvMagnitude() { return math::fast_inv_sqrt<_steps>(this->magnitude2()); }

    template <std::size_t _steps = 1>
    ICA fastMagnitude() { return 1 / fastInvMagnitude<_steps>(); }

    template <std::size_t _steps = 1>
    ICA fastNormalize() { return ((*this) *= fastInvMagnitude<_steps>()); }

    ICA magnitude() { return 1 / fastInvMagnitude<3>(); }
    ICA normalize() { return ((*this) *= fastInvMagnitude<3>()); }
};

namespace std {

    template <typename Type>
    struct tuple_size<Vector2Flt<Type>> : public integral_constant<std::size_t, 2> {};

    template <size_t _pos, typename Type>
    struct tuple_element<_pos, Vector2Flt<Type>> { using type = Type; };
}



#undef ICA // inline constexpr auto



#endif //UIBUILDER_VECTOR2_HPP
