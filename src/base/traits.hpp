#ifndef DECAYENGINE_TRAITS_HPP
#define DECAYENGINE_TRAITS_HPP

#include <type_traits>
#include "ftl/function_traits.hpp"

#define ICB inline constexpr bool
#define ICS inline constexpr std::size_t

namespace ttr {
    template <bool _EnabledTypes, typename ReturnType = void>
    using enable_for = std::enable_if_t<_EnabledTypes, ReturnType>;


    template <typename Type>
    ICB numbers = std::is_floating_point_v<std::remove_reference_t<Type>> ||
                  std::is_integral_v<std::remove_reference_t<Type>>;

    template <typename Type>
    ICB pod_numbers = std::is_pod_v<Type> && numbers<Type>;

    template <typename T1, typename T2>
    ICB same_types = std::is_same_v<std::remove_reference_t<T1>, std::remove_reference_t<T2>>;


    template <typename Type>
    ICB pods = std::is_pod_v<Type>;

    template <typename Type>
    ICB non_pods = !std::is_pod_v<Type>;



    template <typename Function>
    using return_type_of = typename ftl::function_traits<Function>::return_type;

    template <typename Function, std::size_t _ArgNumber>
    using arg_type_of = typename ftl::function_traits<Function>::template args<_ArgNumber>;

    template <typename Function>
    ICS args_count = ftl::function_traits<Function>::arity;



    namespace details {
        template<typename T>
        struct is_tuple_impl : std::false_type {};

        template<typename... Ts>
        struct is_tuple_impl<std::tuple<Ts...>> : std::true_type {};
    }

    template <typename Type>
    ICB is_tuple_v = details::is_tuple_impl<std::decay_t<Type>>::value;

}

#undef ICB
#undef ICS

#endif //DECAYENGINE_TRAITS_HPP
