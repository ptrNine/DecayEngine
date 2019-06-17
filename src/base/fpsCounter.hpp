#pragma once

#include "ftl/string.hpp"
#include "defines.hpp"
#include "time.hpp"

namespace dtls_fps {
    class FpsCounter {
    public:
        ftl::String to_string_with_update() {
            update();
            return to_string();
        }

        ftl::String to_string() {
            return ftl::String().sprintf("{}", get());
        }

        void update() {
            auto    cur = base::timer().timestamp();
            Float64 dur = (cur - _last_update_ts).sec();
            _last_update_ts = cur;

            durations[current_duration++] = dur;
            current_duration %= 100;

            if (init_durations_count <= 100)
                ++init_durations_count;
        }

        double get() {
            double fps = 0;
            for (int i = 0; i < init_durations_count; ++i)
                fps += durations[i];

            fps = init_durations_count / fps;

            return fps;
        }

    private:
        double durations[100]       = {0};
        int    init_durations_count = 0;
        int    current_duration     = 0;

        base::GlobalTimer::Timestamp _last_update_ts;


        DE_MARK_AS_SINGLETON(FpsCounter);
    };

    inline FpsCounter::FpsCounter(): _last_update_ts(base::timer().timestamp()) {}
    inline FpsCounter::~FpsCounter() = default;
}

namespace base {
    auto& fps_counter() {
        return dtls_fps::FpsCounter::instance();
    }
}