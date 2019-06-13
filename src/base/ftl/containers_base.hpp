#ifndef DECAYENGINE_CONTAINERS_BASE_HPP
#define DECAYENGINE_CONTAINERS_BASE_HPP

#include <iostream>

#include <fmt/format.h>
extern "C" {
#include <xxhash.h>
}

#include "../baseTypes.hpp"
#include "../traits.hpp"

namespace ftl {

    template<typename Iter, typename Function>
    auto _iter_reduce(Iter first, Iter last, Function callback, ttr::return_type_of<Function> init)
    -> ttr::return_type_of<Function>
    {
        static_assert(
                ttr::args_count<Function> == 2 ||
                ttr::args_count<Function> == 3,
                "Callback has wrong number of arguments"
        );

        if constexpr (ttr::args_count<Function> == 2) {
            for (; first != last; ++first)
                init = callback(init, *first);
        }
        else if constexpr (ttr::args_count<Function> == 3) {
            for (std::size_t i = 0; first != last; ++first)
                init = callback(init, *first, i++);
        }
        return init;
    }

    template<typename Iter, typename Function>
    auto _iter_reduce(Iter first, Iter last, Function callback)
    -> ttr::enable_for<
            (ttr::numbers<decltype(*first)> &&
            ttr::numbers<ttr::return_type_of<Function>>) ||
            ttr::same_types<decltype(*first), ttr::return_type_of<Function>>,
    ttr::return_type_of<Function>>
    {
        static_assert(
                ttr::args_count<Function> == 2 ||
                ttr::args_count<Function> == 3,
                "Callback has wrong number of arguments"
        );

        ttr::return_type_of<Function> init = *first++;

        if constexpr (ttr::args_count<Function> == 2) {
            for (; first != last; ++first)
                init = callback(init, *first);
        }
        else if constexpr (ttr::args_count<Function> == 3) {
            for (std::size_t i = 1; first != last; ++first)
                init = callback(init, *first, i++);
        }
        return init;
    }

    template<typename Iter, typename Function>
    auto _iter_reduce(Iter first, Iter last, Function callback)
    -> ttr::enable_for<
            (!ttr::numbers<ttr::return_type_of<Function>> &&
            !ttr::same_types<decltype(*first), ttr::return_type_of<Function>>) ||
            (!ttr::numbers<decltype(*first)> &&
            std::is_default_constructible_v<ttr::return_type_of<Function>>),
    ttr::return_type_of<Function>>
    {
        static_assert(
                ttr::args_count<Function> == 2 ||
                ttr::args_count<Function> == 3,
                "Callback has wrong number of arguments"
        );

        auto init = ttr::return_type_of<Function>();

        if constexpr (ttr::args_count<Function> == 2) {
            for (; first != last; ++first)
                init = callback(init, *first);
        }
        else if constexpr (ttr::args_count<Function> == 3) {
            for (std::size_t i = 0; first != last; ++first)
                init = callback(init, *first, i++);
        }
        return init;
    }

    template <typename CIterT>
    void _iter_print(CIterT cbegin, CIterT cend, SizeT size, std::ostream& os = std::cout) {
        switch (size) {
            case 0: os << "{}"; return;
            case 1: os << "{ " << *cbegin << " }"; return;
            default:
                os << "{ " << *cbegin++;
                for (; cbegin != cend; ++cbegin)
                    os << ", " << *cbegin;
                os << " }";
        }
    }
}



#endif //DECAYENGINE_CONTAINERS_BASE_HPP
