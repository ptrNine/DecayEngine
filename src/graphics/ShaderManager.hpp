#pragma once

#include <flat_hash_map.hpp>

#include <glm/glm.hpp>

#include "ftl/string.hpp"
#include "defines.hpp"

namespace grx {
    enum class ShaderType {
        Vertex,
        Fragment
    };
    class ShaderEffect;
}

namespace grx_sl {
    class ShaderManager {
        friend grx::ShaderEffect;
    public:
        unsigned loadEffect(const char* effect_path);
        unsigned loadShader(const char *shader_path, grx::ShaderType shaderType);
        unsigned load      (const char *vertex_shader_path, const char *fragment_shader_path);

    protected:
        ska::flat_hash_map<std::string, unsigned> programs;
        ska::flat_hash_map<std::string, unsigned> effects;
        ftl::String shaders_dir;

        DE_MARK_AS_SINGLETON(ShaderManager);
    };
} // namespace grx_sl

namespace grx {
    class ShaderProgram {
    public:
        explicit ShaderProgram(unsigned glProgramId): _id(glProgramId) {}
        ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

        void makeCurrent();

        unsigned id() const { return _id; }

        int getUniformId (const char* name) const;
        int uniformId    (const char* name);

        // Todo: impl with static assert for wrong type
        template <typename... T>
        void uniform(int ID, const T&... values);

        template <typename... T>
        void uniform(const char* name, const T&... values) {
            uniform(uniformId(name), values...);
        }

    protected:
        unsigned _id;
        ska::flat_hash_map<std::string, int> uniforms;
    };


    class ShaderEffect {
        using inherited = ShaderProgram;
    public:
        explicit ShaderEffect(unsigned glfxEffectId): _effect_id(glfxEffectId) {}
        explicit ShaderEffect(const char* effect_path);

        ShaderProgram compileProgram(const char* glslFunctionName);
    protected:
        unsigned _effect_id;
    };

} // namespace grx





// Todo: specialize uniform template for all types
// Uniform template method

#define GENERATE_UNIFORM(TYPE) \
template <> \
void grx::ShaderProgram::uniform<TYPE>(int, const TYPE&); \
template <> \
void grx::ShaderProgram::uniform<TYPE, TYPE>(int, const TYPE&, const TYPE&); \
template <> \
void grx::ShaderProgram::uniform<TYPE, TYPE, TYPE>(int, const TYPE&, const TYPE&, const TYPE&); \
template <> \
void grx::ShaderProgram::uniform<TYPE, TYPE, TYPE, TYPE> \
        (int, const TYPE&, const TYPE&, const TYPE&, const TYPE&) \


GENERATE_UNIFORM(float);
GENERATE_UNIFORM(double);
GENERATE_UNIFORM(int);
GENERATE_UNIFORM(unsigned int);

#undef GENERATE_UNIFORM

template <>
void grx::ShaderProgram::uniform<glm::mat4>(int, const glm::mat4&);
template <>
void grx::ShaderProgram::uniform<glm::vec3>(int, const glm::vec3&);


namespace grx {
    inline auto &shader_manager() {
        return grx_sl::ShaderManager::instance();
    }
}