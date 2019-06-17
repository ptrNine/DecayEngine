#include "TextureManager.hpp"

#include <fstream>

#include <GLFW/glfw3.h>
#include <IL/il.h>
#include <cstring>
#include <iostream>

#include "configs.hpp"
#include "logs.hpp"


std::string ilGetErrorString() {
    switch (ilGetError()) {
        case IL_INVALID_ENUM:         return "Invalid enum";
        case IL_OUT_OF_MEMORY:        return "Out of memory";
        case IL_FORMAT_NOT_SUPPORTED: return "Format not supported";
        case IL_INTERNAL_ERROR:       return "Internal error";
        case IL_INVALID_VALUE:        return "Invalid value";
        case IL_ILLEGAL_OPERATION:    return "Illegal operation";
        case IL_ILLEGAL_FILE_VALUE:   return "Illegal file value";
        case IL_INVALID_FILE_HEADER:  return "Invalid file header";
        case IL_INVALID_PARAM:        return "Invalid param";
        case IL_COULD_NOT_OPEN_FILE:  return "Could not open file";
        case IL_INVALID_EXTENSION:    return "Invalid extension";
        case IL_FILE_ALREADY_EXISTS:  return "File already exists";
        case IL_OUT_FORMAT_SAME:      return "Out format same";
        case IL_STACK_OVERFLOW:       return "Stack overflow";
        case IL_STACK_UNDERFLOW:      return "Stack underflow";
        case IL_INVALID_CONVERSION:   return "Invalid conversion";
        case IL_BAD_DIMENSIONS:       return "Bad dimensions";
        case IL_FILE_READ_ERROR:      return "File read/write error";
        default:                      return "No error";
    }
}

unsigned grx_txtr::TextureManager::loadIL(const std::string& path) {
    auto realPath = base::fs::to_data_path(base::cfg::read<ftl::String>("textures_dir") / path);

    auto imgID = ilGenImage();
    GLuint texId = 0; glGenTextures(1, &texId);

    ilBindImage(imgID);
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    auto rc = ilLoadImage(realPath.c_str());

    if (!rc) {
        base::Log("Can't load texture '{}'. IL error: {}", realPath, ilGetErrorString());
        ilDeleteImage(imgID);
        glDeleteTextures(1, &texId);
        return 0;
    } else {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

        glBindTexture(GL_TEXTURE_2D, texId);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     ilGetData());

        ilDeleteImage(imgID);\
        return texId;
    }
}

unsigned grx_txtr::TextureManager::loadTexture(const std::string& path) {
    return loadIL(path);
}

grx_txtr::TextureManager:: TextureManager() {
    ilInit();
}

grx_txtr::TextureManager::~TextureManager() {
    for (auto& t : textures)
        glDeleteTextures(1, &(t.second.id));
}

unsigned grx_txtr::TextureManager::load(const std::string& path) {
    auto find = textures.find(path);

    if (find != textures.end()) {
        find->second.count++;
        return find->second.id;
    }
    else {
        auto id = loadTexture(path);

        if (id)
            textures[path].id = id;

        return id;
    }
}

void grx_txtr::TextureManager::destroy(const std::string& path) {
    auto find = textures.find(path);

    if (find != textures.end()) {
        if (find->second.count > 1)
            find->second.count--;
        else {
            glDeleteTextures(1, &find->second.id);
            textures.erase(path);
        }
    }
}

void grx::Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, glID);
}