#pragma once

#include "ftl/string.hpp"

namespace base::fs {
    auto current_path() -> ftl::String;
    void create_dir  (const std::string_view&);
}