#ifndef DECAYENGINE_CONCEPTS_HPP
#define DECAYENGINE_CONCEPTS_HPP

#include <type_traits>

#define CONCEPTS_MEMBER_CHECKER(METHOD_NAME)                                     \
template <typename T>                                                            \
struct _MethodChecker_##METHOD_NAME {                                            \
    using t = char[1];                                                           \
    using f = char[2];                                                           \
    template <typename C> static constexpr t& test(decltype(&C::METHOD_NAME));   \
    template <typename C> static constexpr f& test(...);                         \
    static constexpr bool val = sizeof(test<T>(0)) == sizeof(t);                 \
}

#define CONCEPTS_MEMBER_EXISTS(CLASS, METHOD_NAME) _MethodChecker_##METHOD_NAME<CLASS>::val

namespace concepts {

    template <typename T, typename ... Types>
    constexpr bool any_of = (std::is_same_v<T, Types> || ...);

    template <typename T>
    constexpr bool floats = std::is_floating_point_v<std::remove_const_t<std::remove_reference_t<T>>>;

    template <typename T>
    constexpr bool integers = std::is_integral_v<std::remove_const_t<std::remove_reference_t<T>>>;

    template <typename T>
    constexpr bool numbers = integers<T> || floats<T>;

}

#endif //DECAYENGINE_CONCEPTS_HPP
