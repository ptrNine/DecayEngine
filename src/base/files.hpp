#ifndef DECAYENGINE_FILES_HPP
#define DECAYENGINE_FILES_HPP

#include <iostream>
#include <fstream>
#include <mutex>
#include <string_view>

#include "concepts.hpp"
#include "serialization.hpp"
#include "baseTypes.hpp"

namespace base {
    class FileWriter {
    public:
        FileWriter(const std::string_view& name);

        template <SizeT _Size>
        void write_bytes(const ftl::Array<Byte, _Size>& array) {
            _ofs.write(reinterpret_cast<const char*>(array.data()), _Size);
        }

        void write_bytes(const ftl::Vector<Byte>& bytes) {
            _ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }

        template <typename T>
        void write(const T& value) {
            write_bytes(srlz::serialize(value));
        }

    protected:
        std::ofstream _ofs;
        std::mutex    _mutex;
    };

} // namespace base


#endif //DECAYENGINE_FILES_HPP
