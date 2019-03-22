#ifndef DECAYENGINE_TIME_HPP
#define DECAYENGINE_TIME_HPP

#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <chrono>
#include <ctime>
#include <iostream>
#include <fmt/formatTest.h>
#include "baseTypes.hpp"
#include "ftl/string.hpp"

namespace base {

    struct SDateTimePoint {
        SDateTimePoint(const std::tm& rTime, U32 rMs);

        void print     (std::ostream& os = std::cout,
                        const ftl::String& format = "DD.MM.YYYY hh:mm:ss") const;
        auto to_string (const ftl::String& format = "DD.MM.YYYY hh:mm:ss") const -> ftl::String;

        friend auto operator<< (std::ostream& os, const SDateTimePoint& tp) -> std::ostream& {
            tp.print(os);
            return os;
        }

        U32 ms;
        U32 sec;
        U32 min;
        U32 hour;
        U32 day;
        U32 month;
        U32 year;
    };

    class GlobalTimer {
        using SteadyT     = std::chrono::steady_clock;
        using TimePointT  = SteadyT::time_point;

    public:

        class TimeDuration {
        public:
            explicit
            TimeDuration(const std::chrono::duration<intmax_t, std::nano>& d): _duration(d) {}

            Float64 sec  () const { return std::chrono::duration<Float64>(_duration).count(); }
            S64     milli() const { return std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count(); }
            S64     micro() const { return std::chrono::duration_cast<std::chrono::microseconds>(_duration).count(); }
            S64     nano () const { return _duration.count(); }

        private:
            std::chrono::duration<intmax_t, std::nano> _duration;
        };


        class Timestamp {
        private:
            TimePointT _timestamp;

        public:
            explicit
            Timestamp(const TimePointT& t): _timestamp(t) {}

            TimeDuration operator-(const Timestamp& ts) const {
                return TimeDuration(_timestamp - ts._timestamp);
            }
        };

        auto timestamp        () -> Timestamp { return Timestamp(SteadyT::now()); }
        auto getSystemDateTime() -> SDateTimePoint;



        // Singleton impl

    private:
        GlobalTimer() = default;
        ~GlobalTimer() = default;

    public:
        GlobalTimer(const GlobalTimer&) = delete;
        GlobalTimer& operator= (const GlobalTimer&) = delete;

    public:
        static GlobalTimer&	_instance() { static GlobalTimer instanse; return instanse; }
    };


    // Global functions
    inline GlobalTimer&	timer() { return GlobalTimer::_instance(); }

    inline void sleep(U32 milliseconds) {
        auto cur = timer().timestamp();
        while ((timer().timestamp() - cur).milli() < milliseconds);
    }

}

namespace fmt {
    template<>
    struct formatter<base::SDateTimePoint> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const base::SDateTimePoint& date, FormatContext& ctx) {
            return format_to(ctx.out(), "{}", date.to_string());
        }
    };
}



#endif //_TIMER_HPP_

#endif //DECAYENGINE_TIME_HPP
