#pragma once

#include <string_view>
#include <future>
#include <flat_hash_map.hpp>

#include "defines.hpp"

namespace grx_txtr {

    struct TextureParam {
        unsigned    id    = 0;
        std::size_t count = 1;
    };

    class TextureManager {
    public:
        unsigned load   (const std::string& path);
        void     destroy(const std::string& path);

    protected:
        static unsigned loadIL      (const std::string& path);
        static unsigned loadTexture (const std::string& path);

        ska::flat_hash_map<std::string, TextureParam> textures;

    DE_MARK_AS_SINGLETON(TextureManager);
    };

} // namespace grx_txtr


namespace grx {
    inline auto& texture_manager() {
        return grx_txtr::TextureManager::instance();
    }

    class Texture {
    public:
        Texture() = default;

        explicit Texture(const std::string& path): _name(path) {
            glID = texture_manager().load(path);
        }

        Texture(Texture&& t) noexcept: _name(std::move(t._name)), glID(t.glID) {
            t.glID = 0; // invalidate
        }

        ~Texture() {
            if (glID)
                texture_manager().destroy(_name);
        }

        void bind();

        DE_DEFINE_GET(_name, name);
        bool valid() { return glID != 0; }

    private:
        std::string _name;
        unsigned    glID = 0;
    };
} // namespace grx