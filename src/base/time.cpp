#include <sstream>
#include <iomanip>
#include "time.hpp"

base::SDateTimePoint::SDateTimePoint(const std::tm &rTime, U32 rMs)  {
    ms      = rMs;
    sec     = static_cast<U32>(rTime.tm_sec);
    min     = static_cast<U32>(rTime.tm_min);
    hour    = static_cast<U32>(rTime.tm_hour);
    day     = static_cast<U32>(rTime.tm_mday);
    month   = static_cast<U32>(rTime.tm_mon + 1);
    year    = static_cast<U32>(rTime.tm_year + 1900);
}

auto base::SDateTimePoint::to_string(const ftl::String& format) const -> ftl::String {
    std::stringstream ss;
    print(ss, format);
    return ftl::String(ss.str());
}

void base::SDateTimePoint::print(std::ostream& os, const ftl::String& format) const {
    ftl::String formats = "DMYhmsx";
    int   counter    = 0;
    Char8 lastFormat = '\0';

    for (SizeT i = 0; i < format.length() + 1; ++i) {
        Char8 c = format[i];
        if (lastFormat == c)
            ++counter;
        else {
            switch (lastFormat) {
                case 'D': os << std::setfill('0') << std::setw(counter) << day;   break;
                case 'M': os << std::setfill('0') << std::setw(counter) << month; break;
                case 'Y': os << std::setfill('0') << std::setw(counter) << year;  break;
                case 'h': os << std::setfill('0') << std::setw(counter) << hour;  break;
                case 'm': os << std::setfill('0') << std::setw(counter) << min;   break;
                case 's': os << std::setfill('0') << std::setw(counter) << sec;   break;
                case 'x': os << std::setfill('0') << std::setw(counter) << ms;    break;
                default:               break;
            }

            lastFormat = c;
            counter    = 1;
        }

        if (formats.find_first_of(c) == ftl::String::npos && c != '\0')
            os << c;
    }
}


auto base::GlobalTimer::getSystemDateTime() -> SDateTimePoint  {
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
    std::chrono::duration<intmax_t, std::nano> time = tp.time_since_epoch();
    auto now = time.count();
    now /= 1000000;
    std::time_t res =  std::chrono::system_clock::to_time_t(tp);

    std::tm res2 = *localtime(&res);
    return SDateTimePoint(res2, static_cast<U32>(now % 1000));
}