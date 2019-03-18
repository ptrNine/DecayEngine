#ifndef PTRNINE_FLAGS_HPP
#define PTRNINE_FLAGS_HPP

#include <type_traits>
#include <cstdint>

#define IC inline constexpr

namespace base {



template <typename Type>
using UnsignedInt = std::enable_if_t<std::is_integral_v<Type> && std::is_unsigned_v<Type>>;

template <typename Type, typename = UnsignedInt<Type>>
class Flags {
public:
    template <Type flag, typename = std::enable_if_t<(flag < sizeof(Type) * 8), Type>>
    inline static constexpr Type def = 1 << flag;

public:
    Flags(): _data(0) {}

    IC void set    (Type flags) noexcept       { _data |= flags; }
    IC void set_if (Type flags, bool expr)     { if (expr) set(flags); }
    IC bool test   (Type flags) const noexcept { return _data & flags; }
    IC void reset  () noexcept                 { _data = 0; }
    IC auto data   () const noexcept -> const Type& { return _data; }

private:
    Type _data;
};



} // namespace base

#undef IC // inline constexpr
#endif //PTRNINE_FLAGS_HPP
