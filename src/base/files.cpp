#include "files.hpp"
#include "assert.hpp"

namespace fs = std::filesystem;

base::FileWriter::FileWriter(const std::string_view& name, bool noLog)  {
    auto lock = std::lock_guard(_mutex);

    auto path   = fs::path(name);
    auto parent = path.parent_path();

    std::error_code code;

    if (!parent.empty() && !fs::exists(parent))
        fs::create_directories(parent, code);

    if (code) {
        if (noLog) {
            std::cerr << "Can't create directories at path \'" << parent.string() << "\'\n" << code.message() << std::endl;
            std::abort();
        } else {
            RABORTF("Can't create directories at path \'{}\'", parent.string());
        }
    }

    _ofs.open(name.data(), std::ios_base::binary | std::ios_base::out);

    if (!_ofs.is_open()) {
        if (noLog) {
            std::cerr << "Can't open file: \'" << name << "\'" << std::endl;
            std::abort();
        } else {
            RABORTF("Can't open file: \'{}\'", name);
        }
    }
}


base::FileReader::FileReader(const std::string_view &name, bool noLog) {
    _ifs.open(name.data(), std::ios_base::binary | std::ios_base::in);

    if (!_ifs.is_open()) {
        if (noLog) {
            std::cerr << "Can't open file: \'" << name << "\'" << std::endl;
            std::abort();
        } else {
            RABORTF("Can't open file: \'{}\'", name);
        }
    }

    _ifs.seekg(0, std::ios_base::end);
    _size = static_cast<SizeT>(_ifs.tellg());
    _ifs.seekg(0, std::ios_base::beg);
}

auto base::FileReader::readAllToBytes() -> ftl::Vector<Byte> {
    auto vec = ftl::Vector<Byte>();
    vec.resize(_size);

    _ifs.read(reinterpret_cast<char*>(vec.data()), _size);
    return vec;
}

auto base::FileReader::readAllToString() -> ftl::String {
    auto str = ftl::String();
    str.resize(_size + 1);

    _ifs.read(str.data(), _size);
    return std::move(str);
}

void base::writeBytesToFile(const fs::path& path, const ftl::Vector<Byte>& bytes, bool noLog) {
    auto fw = FileWriter(path.string(), noLog);
    fw.write(bytes);
}

void base::writeBytesToFile(const fs::path& path, const Byte* bytes, SizeT size, bool noLog) {
    auto fw = FileWriter(path.string(), noLog);
    fw.write(bytes, size);
}

auto base::readFileToBytes(const fs::path& path, bool noLog) -> ftl::Vector<Byte> {
    auto fr = FileReader(path.string(), noLog);
    return std::move(fr.readAllToBytes());
}

auto base::readFileToString(const fs::path& path, bool noLog) -> ftl::String {
    auto fr = FileReader(path.string(), noLog);
    return std::move(fr.readAllToString());
}