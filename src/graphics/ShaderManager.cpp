#include "ShaderManager.hpp"

#include <fstream>

#include <GL/glew.h>
#include <GL/glfx.h>
#include <iostream>

#include "filesystem.hpp"
#include "configs.hpp"

grx_sl::ShaderManager::ShaderManager() {
    shaders_dir = base::cfg::read<std::string>("shaders_dir");
}

grx_sl::ShaderManager::~ShaderManager() {
    for (auto& p : programs)
        glDeleteProgram(p.second);

    for (auto& e : effects)
        glfxDeleteEffect(static_cast<GLint>(e.second));
}

unsigned int grx_sl::ShaderManager::loadShader(const char* sp, grx::ShaderType st) {
    unsigned shType = GL_VERTEX_SHADER;
    switch (st) {
        case grx::ShaderType::Fragment: shType = GL_FRAGMENT_SHADER; break;
        case grx::ShaderType::Vertex:   shType = GL_VERTEX_SHADER;   break;
        default: break; // todo: assert
    }

    GLuint ID = glCreateShader(shType);

    // Todo: assert if can't open!
    std::ifstream shIfs(sp, std::ios::in | std::ios::ate);
    auto shIfsEnd = shIfs.tellg();
    shIfs.seekg(0, std::ios::beg);
    auto shCodeSize = shIfsEnd - shIfs.tellg();

    char* shCode = new char[shCodeSize + 1];
    shCode[shCodeSize] = '\0';
    shIfs.read(shCode, shCodeSize);
    shIfs.close();

    // Todo: compilation log
    glShaderSource(ID, 1, &shCode, nullptr);
    glCompileShader(ID);


    GLint rc = GL_FALSE;
    int infoLogLength;

    // Todo: check log
    glGetShaderiv(ID, GL_COMPILE_STATUS, &rc);
    glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* errorMsg = new char[infoLogLength + 1];
        errorMsg[infoLogLength] = '\0';
        glGetShaderInfoLog(ID, infoLogLength, nullptr, errorMsg);
        fprintf(stderr, "%sn", errorMsg);
        delete [] errorMsg;
    }

    delete [] shCode;

    return ID;
}

unsigned int grx_sl::ShaderManager::load(const char* vsp, const char* fsp) {
    auto vRealPath = base::fs::to_data_path(shaders_dir / std::string_view(vsp));
    auto fRealPath = base::fs::to_data_path(shaders_dir / std::string_view(fsp));
    auto key       = vRealPath + fRealPath;

    auto found = programs.find(key);
    if (found != programs.end())
        return found->second;

    auto vsID = loadShader(vRealPath.c_str(), grx::ShaderType::Vertex);
    auto fsID = loadShader(fRealPath.c_str(), grx::ShaderType::Fragment);

    // Todo: log: create shader program
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vsID);
    glAttachShader(ID, fsID);
    glLinkProgram(ID);

    GLint rc = GL_FALSE;
    int infoLogLength;

    // Todo: check log
    glGetProgramiv(ID, GL_LINK_STATUS, &rc);
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* errorMsg = new char[infoLogLength + 1];
        errorMsg[infoLogLength] = '\0';
        glGetProgramInfoLog(ID, infoLogLength, nullptr, errorMsg);
        fprintf(stderr, "%sn", errorMsg);
        delete [] errorMsg;
    }

    glDeleteShader(vsID);
    glDeleteShader(fsID);

    programs.emplace(key, ID);

    return ID;
}

unsigned grx_sl::ShaderManager::loadEffect(const char* ep) {
    auto realPath = base::fs::to_data_path(shaders_dir + std::string_view(ep));
    auto found    = effects.find(realPath);

    if (found != effects.end())
        return found->second;

    auto effectId = glfxGenEffect();
    auto rc = glfxParseEffectFromFile(effectId, realPath.c_str());

    // Todo: assert
    if (!rc)
        std::cerr << "Error creating effect from file '" << realPath << "': "
                  << glfxGetEffectLog(effectId) << std::endl;

    return effects[realPath] = static_cast<GLuint>(effectId);
}

grx::ShaderProgram::ShaderProgram(const char* vsp, const char* fsp) {
    _id = shader_manager().load(vsp, fsp);
}


int grx::ShaderProgram::getUniformId(const char* name) const {
    auto found = uniforms.find(name);

    RASSERTF(found != uniforms.end(), "Can't find '{}' uniform!", name);

    return found->second;
}

int grx::ShaderProgram::uniformId(const char* name) {
    auto find = uniforms.find(name);
    if (find != uniforms.end())
        return find->second;
    else {
        int id = glGetUniformLocation(_id, name);
        RASSERTF(id != -1, "Invalid uniform '{}' location", name);
        return uniforms[name] = id;
    }
}

void grx::ShaderProgram::makeCurrent() {
    glUseProgram(_id);
}

grx::ShaderEffect::ShaderEffect(const char* ep){
    _effect_id = shader_manager().loadEffect(ep);
}

auto grx::ShaderEffect::compileProgram(const char* fn) -> grx::ShaderProgram {
    auto found = shader_manager().programs.find(fn);

    if (found != shader_manager().programs.end())
        return ShaderProgram(found->second);

    auto programId = glfxCompileProgram(_effect_id, fn);

    // Todo: assert!
    if (programId < 0)
        std::cerr << "Error compiling program with '" << fn << "' entry: " <<
                     glfxGetEffectLog(_effect_id) << std::endl;

    shader_manager().programs.emplace(fn, static_cast<GLuint>(programId));
    return ShaderProgram(static_cast<GLuint>(programId));
}


// Todo: specialize uniform template for all types
// Uniform template method specializations

#define GENERATE_UNIFORM_1(TYPE) \
template <> \
void grx::ShaderProgram::uniform(int ID, const TYPE& v1)

#define GENERATE_UNIFORM_2(TYPE) \
template <> \
void grx::ShaderProgram::uniform(int ID, const TYPE& v1, const TYPE& v2)

#define GENERATE_UNIFORM_3(TYPE) \
template <> \
void grx::ShaderProgram::uniform(int ID, const TYPE& v1, const TYPE& v2, const TYPE& v3)

#define GENERATE_UNIFORM_4(TYPE) \
template <> \
void grx::ShaderProgram::uniform(int ID, const TYPE& v1, const TYPE& v2, const TYPE& v3, const TYPE& v4)


GENERATE_UNIFORM_1(float) { glProgramUniform1f(_id, ID, v1); }
GENERATE_UNIFORM_2(float) { glProgramUniform2f(_id, ID, v1, v2); }
GENERATE_UNIFORM_3(float) { glProgramUniform3f(_id, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(float) { glProgramUniform4f(_id, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(int) { glProgramUniform1i(_id, ID, v1); }
GENERATE_UNIFORM_2(int) { glProgramUniform2i(_id, ID, v1, v2); }
GENERATE_UNIFORM_3(int) { glProgramUniform3i(_id, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(int) { glProgramUniform4i(_id, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(unsigned) { glProgramUniform1ui(_id, ID, v1); }
GENERATE_UNIFORM_2(unsigned) { glProgramUniform2ui(_id, ID, v1, v2); }
GENERATE_UNIFORM_3(unsigned) { glProgramUniform3ui(_id, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(unsigned) { glProgramUniform4ui(_id, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(double) { glProgramUniform1d(_id, ID, v1); }
GENERATE_UNIFORM_2(double) { glProgramUniform2d(_id, ID, v1, v2); }
GENERATE_UNIFORM_3(double) { glProgramUniform3d(_id, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(double) { glProgramUniform4d(_id, ID, v1, v2, v3, v4); }


template <>
void grx::ShaderProgram::uniform(int ID, const glm::mat4& val) {
    glProgramUniformMatrix4fv(_id, ID, 1, GL_FALSE, &val[0][0]);
}
template <>
void grx::ShaderProgram::uniform(int ID, const glm::vec3& val) {
    glProgramUniform3f(_id, ID, val.x, val.y, val.z);
}