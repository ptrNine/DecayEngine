#ifndef DECAYENGINE_FILES_HPP
#define DECAYENGINE_FILES_HPP

#include <iostream>
#include <fstream>
#include <mutex>
#include <string_view>
#include <filesystem>

#include "concepts.hpp"
#include "serialization.hpp"
#include "baseTypes.hpp"

// Todo: writer/reader conflicts

namespace base {
    class FileWriter {
    public:
        FileWriter(FileWriter&& fw) noexcept: _ofs(std::move(fw._ofs)) {}
        FileWriter(const std::string_view& name, bool noLog = false);

        template <SizeT _Size>
        auto& write(const ftl::Array<Byte, _Size>& array) {
            auto lock = std::lock_guard(_mutex);
            _ofs.write(reinterpret_cast<const char*>(array.data()), _Size);
            return *this;
        }

        auto& write(const ftl::Vector<Byte>& bytes) {
            auto lock = std::lock_guard(_mutex);
            _ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
            return *this;
        }

        auto& write(const ftl::String& str) {
            auto lock = std::lock_guard(_mutex);
            _ofs.write(str.data(), str.length());
            return *this;
        }

        auto& write(const Byte* buf, SizeT size) {
            auto lock = std::lock_guard(_mutex);
            _ofs.write(reinterpret_cast<const char*>(buf), size);
            return *this;
        }

        template <typename T>
        auto serialize_write(const T& value)
        -> std::enable_if_t<IS_SERIALIZABLE_IMPL(T), FileWriter&> {
            return write(srlz::serialize(value));
        }

        template <typename T>
        auto serialize_write(const T* data, SizeT size)
        -> std::enable_if_t<IS_SERIALIZABLE_IMPL(T), FileWriter&> {
            return write(srlz::serialize(data, size));
        }

        auto& flush() {
            _ofs.flush();
            return *this;
        }

    protected:
        std::ofstream _ofs;
        std::mutex    _mutex;
    };


    class FileReader {
    public:
        FileReader(FileReader&& fr) noexcept
            : _ifs(std::move(fr._ifs)), _size(fr._size) {}

        FileReader(const std::string_view& name, bool noLog = false);

        auto readAllToBytes() -> ftl::Vector<Byte>;
        auto readAllToString() -> ftl::String;

        SizeT size() { return _size; }

    protected:
        SizeT         _size;
        std::ifstream _ifs;
    };


    template <SizeT _Size>
    inline void writeBytesToFile(const std::string_view& path, const ftl::Array<Byte, _Size>& bytes, bool noLog = false) {
        auto fw = FileWriter(path, noLog);
        fw.write(bytes);
    }
    
    void writeBytesToFile(const std::filesystem::path& path, const Byte* bytes, SizeT size, bool noLog = false);
    void writeBytesToFile(const std::filesystem::path& path, const ftl::Vector<Byte>& bytes, bool noLog = false);

    auto readFileToBytes  (const std::filesystem::path& path, bool noLog = false) -> ftl::Vector<Byte>;
    auto readFileToString (const std::filesystem::path& path, bool noLog = false) -> ftl::String;

} // namespace base


#endif //DECAYENGINE_FILES_HPP
