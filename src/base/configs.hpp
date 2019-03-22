#ifndef DECAYENGINE_CONFIGS_HPP
#define DECAYENGINE_CONFIGS_HPP

#include <flat_hash_map.hpp>
#include "ftl/string.hpp"

namespace base {

    class ConfigStorage {

    protected:
        ska::flat_hash_map<ftl::String, int> aa;
    };

} // namespace base

#endif //DECAYENGINE_CONFIGS_HPP
