#include "files.hpp"

#include <filesystem>

namespace fs = std::filesystem;

base::FileWriter::FileWriter(const std::string_view& name)  {
    auto lock = std::lock_guard(_mutex);

    auto path   = fs::path(name);
    auto parent = path.parent_path();

    std::error_code code;

    if (!parent.empty() && !fs::exists(parent))
        fs::create_directories(parent, code);

    if (!code) {
        // Todo: asserts
    }

    _ofs.open(name.data(), std::ios_base::binary | std::ios_base::out);

    if (!_ofs.is_open()) {
        // Todo: asserts
        std::cerr << "Can't open file: '" << name << "'" << std::endl;
    }
}