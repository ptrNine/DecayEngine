#include "files.hpp"
#include "assert.hpp"
#include "filesystem.hpp"

base::FileWriter::FileWriter(const std::string_view& name)  {
    auto lock = std::lock_guard(_mutex);

    auto path   = ftl::String(name);
    auto parent = path.parent_path();

    std::error_code code;

    if (!parent.empty())
        fs::create_dir(parent);

    _ofs.open(name.data(), std::ios_base::binary | std::ios_base::out);

    if (!_ofs.is_open())
        RABORTF("Can't open file: \'{}\'", name);
}


base::FileReader::FileReader(const std::string_view& name) {
    _ifs.open(name.data(), std::ios_base::binary | std::ios_base::in);

    if (!_ifs.is_open())
        RABORTF("Can't open file: \'{}\'", name);

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

void base::writeBytesToFile(const ftl::String& path, const ftl::Vector<Byte>& bytes) {
    auto fw = FileWriter(path);
    fw.write(bytes);
}

void base::writeBytesToFile(const ftl::String& path, const Byte* bytes, SizeT size) {
    auto fw = FileWriter(path);
    fw.write(bytes, size);
}

auto base::readFileToBytes(const ftl::String& path) -> ftl::Vector<Byte> {
    auto fr = FileReader(path);
    return std::move(fr.readAllToBytes());
}

auto base::readFileToString(const ftl::String& path) -> ftl::String {
    auto fr = FileReader(path);
    return std::move(fr.readAllToString());
}