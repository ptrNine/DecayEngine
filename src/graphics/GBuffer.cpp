#include "GBuffer.hpp"

#include <GL/glew.h>
#include <iostream>

grx::GBuffer::GBuffer(unsigned w, unsigned h) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glGenTextures(TextureType_COUNT, textures);
    glGenTextures(1, &depth_texture);
    glGenTextures(1, &final_texture);

    for (unsigned i = 0; i < TextureType_COUNT; ++i) {
        glBindTexture  (GL_TEXTURE_2D, textures[i]);
        glTexImage2D   (GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glBindTexture(GL_TEXTURE_2D, final_texture);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, final_texture, 0);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr <<"framebuffer failed!" << std::endl;
        // Todo: assert
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

grx::GBuffer::~GBuffer() {
    glDeleteFramebuffers(1, &fbo);

    glDeleteTextures(TextureType_COUNT, textures);
    glDeleteTextures(1, &depth_texture);
    glDeleteTextures(1, &final_texture);
}


void grx::GBuffer::start() {
    // Clear final texture
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT); //
}

void grx::GBuffer::geometryPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(TextureType_COUNT, drawBuffers);
}

void grx::GBuffer::stencilPass() {
    // Disable draw buffers
    glDrawBuffer(GL_NONE);
}

void grx::GBuffer::lightPass() {
    glDrawBuffer(GL_COLOR_ATTACHMENT4);

    for (unsigned i = 0; i < TextureType_COUNT; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture  (GL_TEXTURE_2D, textures[i]);
    }
}

void grx::GBuffer::finalize() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}