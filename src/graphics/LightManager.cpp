#include "LightManager.hpp"

#include "ShaderManager.hpp"

dtls_light::LightManager::LightManager() {
    _directional_light.is_active() = false;

    for (auto& l : _point_lights)
        l.is_active() = false;

    for (auto& l : _spot_lights)
        l.is_active() = false;

    // Generate light parameter names in shaders
    for (int i = 0; i < FR_MAX_POINT_LIGHTS; ++i) {
        _point_lights_names[i].color                 = ftl::String().sprintf("_point_lights[{}].base.color", i);
        _point_lights_names[i].position              = ftl::String().sprintf("_point_lights[{}].position", i);
        _point_lights_names[i].ambient_intensity     = ftl::String().sprintf("_point_lights[{}].base.ambient_intensity", i);
        _point_lights_names[i].diffuse_intensity     = ftl::String().sprintf("_point_lights[{}].base.diffuse_intensity", i);
        _point_lights_names[i].attenuation_constant  = ftl::String().sprintf("_point_lights[{}].attenuation.constant", i);
        _point_lights_names[i].attenuation_linear    = ftl::String().sprintf("_point_lights[{}].attenuation.linear", i);
        _point_lights_names[i].attenuation_quadratic = ftl::String().sprintf("_point_lights[{}].attenuation.quadratic", i);
    }

    for (int i = 0; i < FR_MAX_SPOT_LIGHTS; ++i) {
        _spot_lights_names[i].color                 = ftl::String().sprintf("_spot_lights[{}].base.base.color", i);
        _spot_lights_names[i].position              = ftl::String().sprintf("_spot_lights[{}].base.position", i);
        _spot_lights_names[i].ambient_intensity     = ftl::String().sprintf("_spot_lights[{}].base.base.ambient_intensity", i);
        _spot_lights_names[i].diffuse_intensity     = ftl::String().sprintf("_spot_lights[{}].base.base.diffuse_intensity", i);
        _spot_lights_names[i].attenuation_constant  = ftl::String().sprintf("_spot_lights[{}].base.attenuation.constant", i);
        _spot_lights_names[i].attenuation_linear    = ftl::String().sprintf("_spot_lights[{}].base.attenuation.linear", i);
        _spot_lights_names[i].attenuation_quadratic = ftl::String().sprintf("_spot_lights[{}].base.attenuation.quadratic", i);
        _spot_lights_names[i].direction             = ftl::String().sprintf("_spot_lights[{}].direction", i);
        _spot_lights_names[i].cutoff                = ftl::String().sprintf("_spot_lights[{}].cutoff", i);
    }
}

dtls_light::LightManager::~LightManager() = default;


void dtls_light::LightManager::assignTo(grx::ShaderProgram &program) {
    auto found = _cached_program_uniforms.find(program.id());

    if (found != _cached_program_uniforms.end())
        _pushToShader(program, found->second);
    else {
        _cached_program_uniforms[program.id()] = _genUniformIDs(program);
        _pushToShader(program, _cached_program_uniforms[program.id()]);
    }
}

void dtls_light::LightManager::_pushToShader(grx::ShaderProgram& program, const CachedUniforms& uniforms) {
    int plc = 0; // point light counter
    int slc = 0; // spot light counter

    program.uniform(uniforms.specular_power,     _specular_power);
    program.uniform(uniforms.specular_intensity, _specular_intensity);

    // Directional light
    if (_directional_light.is_active()) {
        program.uniform(uniforms.dir_light_uniforms.color,             _directional_light.color());
        program.uniform(uniforms.dir_light_uniforms.diffuse_intensity, _directional_light.diffuse_intensity());
        program.uniform(uniforms.dir_light_uniforms.ambient_intensity, _directional_light.ambient_intensity());
        program.uniform(uniforms.dir_light_uniforms.direction,         _directional_light.direction());
    }

    // Point lights
    for (SizeT i = 0; i < FR_MAX_POINT_LIGHTS; ++i) {
        if (_point_lights[i].is_active()) {
            program.uniform(uniforms.point_light_uniforms[plc].color,                 _point_lights[i].color());
            program.uniform(uniforms.point_light_uniforms[plc].position,              _point_lights[i].position());
            program.uniform(uniforms.point_light_uniforms[plc].diffuse_intensity,     _point_lights[i].diffuse_intensity());
            program.uniform(uniforms.point_light_uniforms[plc].ambient_intensity,     _point_lights[i].ambient_intensity());
            program.uniform(uniforms.point_light_uniforms[plc].attenuation_constant,  _point_lights[i].attenuation_constant());
            program.uniform(uniforms.point_light_uniforms[plc].attenuation_linear,    _point_lights[i].attenuation_linear());
            program.uniform(uniforms.point_light_uniforms[plc].attenuation_quadratic, _point_lights[i].attenuation_quadratic());
            ++plc;
        }
    }
    program.uniform(uniforms.point_light_count_uniform, plc);

    // Spot lights
    for (SizeT i = 0; i < FR_MAX_SPOT_LIGHTS; ++i) {
        if (_spot_lights[i].is_active()) {
            program.uniform(uniforms.spot_light_uniforms[slc].color,                 _spot_lights[i].color());
            program.uniform(uniforms.spot_light_uniforms[slc].position,              _spot_lights[i].position());
            program.uniform(uniforms.spot_light_uniforms[slc].diffuse_intensity,     _spot_lights[i].diffuse_intensity());
            program.uniform(uniforms.spot_light_uniforms[slc].ambient_intensity,     _spot_lights[i].ambient_intensity());
            program.uniform(uniforms.spot_light_uniforms[slc].attenuation_constant,  _spot_lights[i].attenuation_constant());
            program.uniform(uniforms.spot_light_uniforms[slc].attenuation_linear,    _spot_lights[i].attenuation_linear());
            program.uniform(uniforms.spot_light_uniforms[slc].attenuation_quadratic, _spot_lights[i].attenuation_quadratic());
            program.uniform(uniforms.spot_light_uniforms[slc].direction,             _spot_lights[i].direction());
            program.uniform(uniforms.spot_light_uniforms[slc].cutoff,                _spot_lights[i].cutoff());
            ++slc;
        }
    }
    program.uniform(uniforms.spot_light_count_uniform, slc);
}

auto dtls_light::LightManager::_genUniformIDs(grx::ShaderProgram& program) -> CachedUniforms {
    CachedUniforms u;

    u.specular_power            = program.uniformId("_specular_power");
    u.specular_intensity        = program.uniformId("_mat_specular_intensity");
    u.point_light_count_uniform = program.uniformId("_num_point_lights");
    u.spot_light_count_uniform  = program.uniformId("_num_spot_lights");

    u.dir_light_uniforms.color             = program.uniformId(_dir_light_names.color);
    u.dir_light_uniforms.diffuse_intensity = program.uniformId(_dir_light_names.diffuse_intensity);
    u.dir_light_uniforms.ambient_intensity = program.uniformId(_dir_light_names.ambient_intensity);
    u.dir_light_uniforms.direction         = program.uniformId(_dir_light_names.direction);

    for (SizeT i = 0; i < FR_MAX_POINT_LIGHTS; ++i) {
        u.point_light_uniforms[i].color                 = program.uniformId(_point_lights_names[i].color.c_str());
        u.point_light_uniforms[i].position              = program.uniformId(_point_lights_names[i].position.c_str());
        u.point_light_uniforms[i].diffuse_intensity     = program.uniformId(_point_lights_names[i].diffuse_intensity.c_str());
        u.point_light_uniforms[i].ambient_intensity     = program.uniformId(_point_lights_names[i].ambient_intensity.c_str());
        u.point_light_uniforms[i].attenuation_constant  = program.uniformId(_point_lights_names[i].attenuation_constant.c_str());
        u.point_light_uniforms[i].attenuation_linear    = program.uniformId(_point_lights_names[i].attenuation_linear.c_str());
        u.point_light_uniforms[i].attenuation_quadratic = program.uniformId(_point_lights_names[i].attenuation_quadratic.c_str());
    }

    for (SizeT i = 0; i < FR_MAX_SPOT_LIGHTS; ++i) {
        u.spot_light_uniforms[i].color                 = program.uniformId(_spot_lights_names[i].color.c_str());
        u.spot_light_uniforms[i].position              = program.uniformId(_spot_lights_names[i].position.c_str());
        u.spot_light_uniforms[i].diffuse_intensity     = program.uniformId(_spot_lights_names[i].diffuse_intensity.c_str());
        u.spot_light_uniforms[i].ambient_intensity     = program.uniformId(_spot_lights_names[i].ambient_intensity.c_str());
        u.spot_light_uniforms[i].attenuation_constant  = program.uniformId(_spot_lights_names[i].attenuation_constant.c_str());
        u.spot_light_uniforms[i].attenuation_linear    = program.uniformId(_spot_lights_names[i].attenuation_linear.c_str());
        u.spot_light_uniforms[i].attenuation_quadratic = program.uniformId(_spot_lights_names[i].attenuation_quadratic.c_str());
        u.spot_light_uniforms[i].direction             = program.uniformId(_spot_lights_names[i].direction.c_str());
        u.spot_light_uniforms[i].cutoff                = program.uniformId(_spot_lights_names[i].cutoff.c_str());
    }

    return u;
}