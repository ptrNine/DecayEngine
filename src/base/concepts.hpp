#ifndef DECAYENGINE_CONCEPTS_HPP
#define DECAYENGINE_CONCEPTS_HPP

#include <type_traits>

namespace concepts {
    template <typename T, typename ... Types>
    constexpr bool any_of = (std::is_same_v<T, Types> || ...);
}

#endif //DECAYENGINE_CONCEPTS_HPP
