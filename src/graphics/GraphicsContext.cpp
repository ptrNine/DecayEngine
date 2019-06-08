#include "GraphicsContext.hpp"

#include <iostream>

#include <GLFW/glfw3.h>

void grx_ctx::GraphicsContext::glfwError(int id, const char *description) {
    std::cerr << description << std::endl;
    exit(id);
}

grx_ctx::GraphicsContext::GraphicsContext() {
    glfwSetErrorCallback(&glfwError);
    glfwInit();

    // Antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

grx_ctx::GraphicsContext::~GraphicsContext() = default;