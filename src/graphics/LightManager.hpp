#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <flat_hash_map.hpp>

#include "defines.hpp"
#include "assert.hpp"
#include "logs.hpp"
#include "ftl/string.hpp"

namespace grx {
    class LightBase {
    protected:
        glm::vec3 _color;
        float     _ambient_intensity;
        float     _diffuse_intensity;
        bool      _is_active = false;

    public:
        DE_DEFINE_GETSET(_color, color);
        DE_DEFINE_GETSET(_ambient_intensity, ambient_intensity);
        DE_DEFINE_GETSET(_diffuse_intensity, diffuse_intensity);
        DE_DEFINE_GETSET_FIELD(_is_active, is_active);
    };


    class DirectionalLight : public LightBase {
    public:
        explicit
        DirectionalLight(const glm::vec3& direction = {0.f, -1.f, 0.f},
                         const glm::vec3& color = {1.f, 1.f, 1.f},
                         float ambient_intencity = 0.1f,
                         float diffuse_intencity = 0.9): _direction(direction)
        {
            _color = color;
            _ambient_intensity = ambient_intencity;
            _diffuse_intensity = diffuse_intencity;
        }

    protected:
        glm::vec3 _direction;

    public:
        DE_DEFINE_GETSET(_direction, direction);
    };


    class PointLight : public LightBase {
    public:
        explicit
        PointLight(const glm::vec3& position = {0.f, 0.f, 0.f},
                   const glm::vec3& color  = {1.f, 1.f, 1.f},
                   float ambient_intencity = 0.1f,
                   float diffuse_intencity = 0.9): _position(position)
        {
            _color = color;
            _ambient_intensity = ambient_intencity;
            _diffuse_intensity = diffuse_intencity;
        }

        void setAttenuation(float constant, float linear, float quadratic) {
            _attenuation_constant  = constant;
            _attenuation_linear    = linear;
            _attenuation_quadratic = quadratic;
        }

    protected:
        glm::vec3 _position;

        float     _attenuation_constant  = 1.f;
        float     _attenuation_linear    = 0.f;
        float     _attenuation_quadratic = 0.f;

    public:
        DE_DEFINE_GETSET(_position, position);
        DE_DEFINE_GETSET(_attenuation_constant,  attenuation_constant);
        DE_DEFINE_GETSET(_attenuation_linear,    attenuation_linear);
        DE_DEFINE_GETSET(_attenuation_quadratic, attenuation_quadratic);
    };


    class SpotLight : public PointLight {
    public:
        explicit
        SpotLight(const glm::vec3& position = {0.f, 0.f, 0.f},
                  const glm::vec3& direction = {0.f, 0.f, -1.f},
                  float cutoff = 1.f,
                  const glm::vec3& color  = {1.f, 1.f, 1.f},
                  float ambient_intencity = 0.1f,
                  float diffuse_intencity = 0.9): _direction(direction), _cutoff(cutoff)
        {
            _color = color;
            _ambient_intensity = ambient_intencity;
            _diffuse_intensity = diffuse_intencity;
        }

    protected:
        glm::vec3 _direction;
        float     _cutoff;

    public:
        DE_DEFINE_GETSET(_direction, direction);
        DE_DEFINE_GETSET(_cutoff, cutoff);
    };
}

namespace grx {
    class DirLightProvider;
    class PointLightProvider;
    class SpotLightProvider;
    class ShaderProgram;
}

namespace dtls_light {
    class LightManager {
        friend grx::DirLightProvider;
        friend grx::PointLightProvider;
        friend grx::SpotLightProvider;

    public:
        static constexpr SizeT FR_MAX_POINT_LIGHTS = 16;
        static constexpr SizeT FR_MAX_SPOT_LIGHTS  = 16;

    public:
        int addPointLight(const grx::PointLight& light) {
            for (int i = 0; i < FR_MAX_POINT_LIGHTS; ++i) {
                if (!_point_lights[i].is_active()) {
                    _point_lights[i] = light;
                    _point_lights[i].is_active() = true;
                    return i;
                }
            }

            base::Log("Attempt to create more then {} point light sources!", FR_MAX_POINT_LIGHTS);
            return FR_MAX_POINT_LIGHTS;
        }

        int addSpotLight(const grx::SpotLight& light) {
            for (int i = 0; i < FR_MAX_SPOT_LIGHTS; ++i) {
                if (!_spot_lights[i].is_active()) {
                    _spot_lights[i] = light;
                    _spot_lights[i].is_active() = true;
                    return i;
                }
            }

            base::Log("Attempt to create more then {} spot light sources!", FR_MAX_SPOT_LIGHTS);
            return FR_MAX_SPOT_LIGHTS;
        }

        void pushToShader(grx::ShaderProgram& program);

    protected:
        grx::DirectionalLight _directional_light;
        std::array<grx::PointLight, FR_MAX_POINT_LIGHTS + 1> _point_lights;
        std::array<grx::SpotLight,  FR_MAX_SPOT_LIGHTS + 1>  _spot_lights;



        struct DirLightNames {
            const char* color             = "_directional_light.base.color";
            const char* ambient_intensity = "_directional_light.base.ambient_intensity";
            const char* diffuse_intensity = "_directional_light.base.diffuse_intensity";
            const char* direction         = "_directional_light.direction";
        } _dir_light_names;

        struct PointLightNames{
            ftl::String color;
            ftl::String position;
            ftl::String ambient_intensity;
            ftl::String diffuse_intensity;
            ftl::String attenuation_constant;
            ftl::String attenuation_linear;
            ftl::String attenuation_quadratic;
        };

        struct SpotLightNames{
            ftl::String color;
            ftl::String position;
            ftl::String ambient_intensity;
            ftl::String diffuse_intensity;
            ftl::String attenuation_constant;
            ftl::String attenuation_linear;
            ftl::String attenuation_quadratic;
            ftl::String direction;
            ftl::String cutoff;
        };

        struct DirLightUniformIDs {
            int color;
            int ambient_intensity;
            int diffuse_intensity;
            int direction;
        };

        struct PointLightUniformIDs {
            int color;
            int position;
            int ambient_intensity;
            int diffuse_intensity;
            int attenuation_constant;
            int attenuation_linear;
            int attenuation_quadratic;
        };

        struct SpotLightUniformIDs {
            int color;
            int position;
            int ambient_intensity;
            int diffuse_intensity;
            int attenuation_constant;
            int attenuation_linear;
            int attenuation_quadratic;
            int direction;
            int cutoff;
        };

        struct CachedUniforms {
            DirLightUniformIDs dir_light_uniforms;
            std::array<PointLightUniformIDs, FR_MAX_POINT_LIGHTS> point_light_uniforms;
            std::array<SpotLightUniformIDs,  FR_MAX_POINT_LIGHTS> spot_light_uniforms;
            int point_light_count_uniform;
            int spot_light_count_uniform;
        };

        std::array<PointLightNames, FR_MAX_POINT_LIGHTS> _point_lights_names;
        std::array<SpotLightNames,  FR_MAX_SPOT_LIGHTS>  _spot_lights_names;

        ska::flat_hash_map<unsigned, CachedUniforms> _cached_program_uniforms;

    protected:
        void _pushToShader (grx::ShaderProgram& program, const CachedUniforms& uniforms);
        auto _genUniformIDs(grx::ShaderProgram& program) -> CachedUniforms;

        DE_MARK_AS_SINGLETON(LightManager);
    };
}


namespace grx {
    inline auto& light_manager() {
        return dtls_light::LightManager::instance();
    }


    class DirLightProvider {
    public:
        explicit DirLightProvider(const DirectionalLight& light) {
            RASSERTF(!light_manager()._directional_light.is_active(),
                    "{}", "Attempt to create two or more directional light sources!");

            light_manager()._directional_light = light;
            light_manager()._directional_light.is_active() = true;
        }

        ~DirLightProvider() {
            light_manager()._directional_light.is_active() = false;
        }
    };

    class PointLightProvider {
    public:
        explicit PointLightProvider(const PointLight& light) {
            id = light_manager().addPointLight(light);
        }

        ~PointLightProvider() {
            light_manager()._point_lights[id].is_active() = false;
        }

        auto& get() {
            return light_manager()._point_lights[id];
        }

    protected:
        int id;
    };

    class SpotLightProvider {
    public:
        explicit SpotLightProvider(const SpotLight& light) {
            id = light_manager().addSpotLight(light);
        }

        ~SpotLightProvider() {
            light_manager()._spot_lights[id].is_active() = false;
        }

        auto& get() {
            return light_manager()._spot_lights[id];
        }

    protected:
        int id;
    };


}