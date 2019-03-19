#ifndef DECAYENGINE_CONCEPTS_HPP
#define DECAYENGINE_CONCEPTS_HPP

#include <type_traits>

namespace concepts {
    template <typename T, typename ... Types>
    constexpr bool any_of = (std::is_same_v<T, Types> || ...);

    template <typename T>
    constexpr bool floats = std::is_floating_point_v<T>;

    template <typename T>
    constexpr bool integers = std::is_integral_v<T>;

    template <typename T>
    constexpr bool numbers = integers<T> || floats<T>;
}

#endif //DECAYENGINE_CONCEPTS_HPP
