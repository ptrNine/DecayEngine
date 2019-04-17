#ifndef DECAYENGINE_ASSERT_HPP
#define DECAYENGINE_ASSERT_HPP

#include <iostream>
#include "assert.h"
#include "defines.hpp"
#include "logs.hpp"


#define RASSERT(EXPR) details::assert_impl((EXPR), __FILE__, __FUNCTION__, __LINE__, #EXPR)
#define RASSERTF(EXPR, FMT, ...) details::assert_impl((EXPR), __FILE__, __FUNCTION__, __LINE__, #EXPR, FMT, __VA_ARGS__)

#define RABORT() RASSERT(false)
#define RABORTF(FMT, ...) RASSERTF(false, FMT, __VA_ARGS__)

#ifdef DE_DEBUG
    #define ASSERT(EXPR) RASSERT(EXPR)
    #define ASSERTF(EXPR, FMT, ...) RASSERTF(EXPR, FMT, __VA_ARGS__)
    #define ABORT() ASSERT(false)
    #define ABORTF(FMT, ...) ASSERTF(false, FMT, __VA_ARGS__)
#else
    #define ASSERT(EXPR) void(0)
    #define ASSERTF(EXPR, FMT, ...) void(0)
    #define ABORT() void(0)
    #define ABORTF(FMT, ...) void(0)
#endif


namespace details {
    inline void assert_impl(
            bool expr,
            const char* file,
            const char* func,
            int line,
            const char* strexp
    ) {
        if (!expr) {
            auto msg = fmt::format("\nFATAL ERROR:\nfile: {}\nfunc: {}\nline: {}\nexpr: {}",
                    file, func, line, strexp);
            base::Log("{}", msg);
            std::cerr << msg << std::endl;
            std::abort();
        }
    }

    template <typename FmtT, typename... ArgT>
    inline void assert_impl(
            bool expr,
            const char* file,
            const char* func,
            int line,
            const char* strexp,
            FmtT format,
            ArgT... args
    ) {
        if (!expr) {
            auto msg = fmt::format("\nFATAL ERROR:\nfile: {}\nfunc: {}\nline: {}\nexpr: {}\nwhat: {}",
                    file, func, line, strexp, fmt::format(format, args...));
            base::Log("{}", msg);
            std::cerr << msg << std::endl;
            std::abort();
        }
    }
}

#endif //DECAYENGINE_ASSERT_HPP
