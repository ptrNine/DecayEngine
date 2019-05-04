#ifndef DECAYENGINE_FILES_HPP
#define DECAYENGINE_FILES_HPP

#include <iostream>
#include <fstream>
#include <mutex>
#include <string_view>

#include "concepts.hpp"
#include "serialization.hpp"
#include "baseTypes.hpp"

// Todo: writer/reader conflicts

namespace base {
    class FileWriter {
    public:
        FileWriter(FileWriter&& fw) noexcept: _ofs(std::move(fw._ofs)) {}
        FileWriter(const std::string_view& name);

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

        FileReader(const std::string_view& name);

        auto readAllToBytes() -> ftl::Vector<Byte>;
        auto readAllToString() -> ftl::String;

        SizeT size() { return _size; }

    protected:
        SizeT         _size;
        std::ifstream _ifs;
    };


    template <SizeT _Size>
    inline void writeBytesToFile(const ftl::String& path, const ftl::Array<Byte, _Size>& bytes) {
        auto fw = FileWriter(path);
        fw.write(bytes);
    }
    
    void writeBytesToFile(const ftl::String& path, const Byte* bytes, SizeT size);
    void writeBytesToFile(const ftl::String& path, const ftl::Vector<Byte>& bytes);

    auto readFileToBytes  (const ftl::String& path) -> ftl::Vector<Byte>;
    auto readFileToString (const ftl::String& path) -> ftl::String;

} // namespace base


#endif //DECAYENGINE_FILES_HPP
