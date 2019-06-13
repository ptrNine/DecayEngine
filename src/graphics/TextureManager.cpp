#include "TextureManager.hpp"

#include <fstream>

#include <GLFW/glfw3.h>
#include <IL/il.h>
#include <cstring>
#include <iostream>

#include "configs.hpp"
#include "logs.hpp"

//#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
//#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
//#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

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

/*
unsigned grx_txtr::TextureManager::loadDDS(const std::string_view& path) {
    std::ifstream img(path.data(), std::ios::in | std::ios::ate);

    // Todo: assert in can't open
    if (!img.is_open()) {
        std::cerr << "Can't open dds texture file " << path << std::endl;
        return 0;
    }

    auto imgEng = img.tellg();
    img.seekg(0, std::ios::beg);
    auto imgSize = imgEng - img.tellg();

    auto imgBytes = new unsigned char[imgSize + 1];
    imgBytes[imgSize] = '\0';
    img.read(reinterpret_cast<char*>(imgBytes), imgSize);
    img.close();

    // Todo: assert if 4 bytes not equal with "DDS "

    unsigned char header[124];
    std::memcpy(header, imgBytes + 4, 124);


    // Todo: endian insensitive reading
    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

    unsigned int bufsize;
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    auto buffer = new unsigned char[bufsize];
    std::memcpy(buffer, imgBytes + 128, bufsize);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            // Todo: assert or error with dummy texture
            delete [] imgBytes;
            delete [] buffer;
            return 0;
    }


    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    for (unsigned l = 0; l < mipMapCount && (width || height); ++l) {
        unsigned size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, l, format, width, height,
                               0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;

        if(width < 1) width = 1;
        if(height < 1) height = 1;
    }

    delete [] imgBytes;
    delete [] buffer;

    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}
 */

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