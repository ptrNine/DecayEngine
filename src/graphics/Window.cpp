#include "Window.hpp"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <flat_hash_map.hpp>

#include "GraphicsContext.hpp"
#include "InputContext.hpp"
#include "Camera.hpp"

// map glfw window pointer to grx window pointer :/
static ska::flat_hash_map<GLFWwindow*, grx::Window*> windowMapping;


grx::Window::Window(const char* n, uint32_t w, uint32_t h) {
    grx::current_context(); // init graphics context

    glfwWindow = glfwCreateWindow(w, h, n, nullptr, nullptr);

    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Todo: assert
    if (!glfwWindow) {
        fprintf(stderr, "Can't create glfw window!");
        glfwTerminate();
    }

    auto savedContext = glfwGetCurrentContext();

    glfwMakeContextCurrent(glfwWindow);

    glewExperimental = GL_TRUE; // required in core mode

    auto rc = glewInit();
    // Todo: assert!
    if (rc != GLEW_OK) {
        std::cerr << glewGetErrorString(rc) << std::endl;
        exit(rc);
    }

    // enable input
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);

    glCullFace (GL_BACK);
    glEnable   (GL_CULL_FACE);
    glEnable   (GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Restore context
    glfwMakeContextCurrent(savedContext);

    // Create default camera
    currentCam = std::make_shared<Camera>();
    currentCam->position().z = -10;
    currentCam->look_at(glm::vec3(0,0,10));

    windowMapping[glfwWindow] = this;

    // set callbacks
    glfwSetWindowFocusCallback(glfwWindow, glfwFocusCallback);

    input::ctx().setDisplaySize(w, h);
}

grx::Window::~Window() {
    windowMapping.erase(glfwWindow);
    glfwDestroyWindow(glfwWindow);
}

grx::Camera* grx::Window::getCamera() {
    return currentCam.get();
}

void grx::Window::setCamera(Camera* camera) {
    currentCam = CameraPtr(camera);
}

void grx::Window::setCamera(const CameraPtr& camera) {
    currentCam = camera;
}

void grx::Window::makeCurrent() {
    glfwMakeContextCurrent(glfwWindow);
}

bool grx::Window::isShouldClose() {
    return static_cast<bool>(glfwWindowShouldClose(glfwWindow));
}

void grx::Window::swapBuffers() {
    glfwSwapBuffers(glfwWindow);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int grx::Window::getKey(int key) {
    return glfwGetKey(glfwWindow, key);
}

bool grx::Window::isKeyPress(int key) {
    return glfwGetKey(glfwWindow, key) == GLFW_PRESS;
}

bool grx::Window::isKeyRelease(int key) {
    return glfwGetKey(glfwWindow, key) == GLFW_RELEASE;
}

glm::vec2 grx::Window::getMousePos() {
    double x, y;
    glfwGetCursorPos(glfwWindow, &x, &y);
    return glm::vec2(x, y);
}

void grx::Window::setMousePos(const glm::vec2& position) {
    glfwSetCursorPos(glfwWindow, position.x, position.y);
}

void grx::Window::resetMousePos() {
    auto size = getSize();
    setMousePos(glm::vec2(size.x / 2, size.y / 2));
}

glm::vec2 grx::Window::getSize() {
    int x, y;
    glfwGetWindowSize(glfwWindow, &x, &y);
    return glm::vec2(x, y);
}

bool grx::Window::isMouseLeftButtonPress() {
    return glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
}

bool grx::Window::isMouseRightButtonPress() {
    return glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
}

grx::Window::VP_T grx::Window::update_view_projection() {
    if (currentCam)
        return currentCam->update_view_projection(this);

    return VP_T(1.f, 1.f);
}

void grx::Window::glfwFocusCallback(GLFWwindow* window, int focused) {
    windowMapping[window]->_onFocus = focused != 0;
}