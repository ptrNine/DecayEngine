#ifndef DECAYENGINE_LAMBDA_HPP
#define DECAYENGINE_LAMBDA_HPP

#include <type_traits>
#include <tuple>

namespace ftl
{
    namespace details
    {
        template<typename Ret, typename IsMutable, typename... Args>
        struct lambda_traits {
            using is_mutable  = IsMutable;
            using return_type = Ret;
            static constexpr auto arity = sizeof...(Args);

            template <std::size_t _index>
            using args = std::tuple_element_t<_index, std::tuple<Args...>>;
        };

        template <typename Ret, typename... Args>
        struct function_traits {
            using return_type = Ret;
            static constexpr auto arity = sizeof...(Args);

            template <std::size_t _index>
            using args = std::tuple_element_t<_index, std::tuple<Args...>>;
        };
    }

    // Lambda
    template<typename Lambda>
    struct function_traits
            : public function_traits<decltype(&Lambda::operator())>
    {};

    template<typename Ret, typename Lambda, typename... Args>
    struct function_traits<Ret(Lambda::*)(Args...)>
            : public details::lambda_traits<Ret, std::true_type, Args...>
    {};

    template<typename Ret, typename Lambda, typename... Args>
    struct function_traits<Ret(Lambda::*)(Args...) const>
            : public details::lambda_traits<Ret, std::false_type, Args...>
    {};


    // Function
    template <typename Ret, typename... Args>
    struct function_traits<Ret(*)(Args...)>
            : public function_traits<Ret(Args...)>
    {};

    template <typename Ret, typename... Args>
    struct function_traits<Ret(Args...)>
            : public details::function_traits<Ret, Args...>
    {};
}

#endif //DECAYENGINE_LAMBDA_HPP
