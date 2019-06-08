#pragma once

#include <string_view>
#include <future>
#include <flat_hash_map.hpp>

#include "defines.hpp"

namespace grx_txtr {

    class TextureManager {
    public:
        unsigned load(const char* path);
        auto     asyncLoad(const char* path) -> std::future<unsigned>;

    protected:
        static unsigned loadDDS     (const std::string_view& path);
        static unsigned loadIL      (const std::string_view& path);
        static unsigned loadTexture (const std::string_view& path);
        static unsigned pushTexture (ska::flat_hash_map<std::string, unsigned>* map, const char* path);

        ska::flat_hash_map<std::string, unsigned> textures;
        std::mutex load_mutex;


    DE_MARK_AS_SINGLETON(TextureManager);
    };

} // namespace grx

namespace grx {
    /**
     * @return Texture manager
     */
    inline auto &texture_manager() {
        return grx_txtr::TextureManager::instance();
    }
} // namespace grx