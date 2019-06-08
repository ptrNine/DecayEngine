#include "InputContext.hpp"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>


#ifdef __linux__
    #include <vector>
    #include <X11/Xlib.h>
#elif _WIN32
    #error "Implement me for Windows!"
#else
    #error "Unknown OS"
#endif


input_impl::InputContext:: InputContext(): x11_display(glfwGetX11Display())  {}
input_impl::InputContext::~InputContext() = default;


void input_impl::InputContext::update() {
    manager.Update();

    #ifdef __linux__
        XEvent event;

        glfw_events.clear();

        while (XPending(x11_display)) {
            XNextEvent(x11_display, &event);
            manager.HandleEvent(event);
            glfw_events.push_back(event);
        }

        // Put back events for glfw :/
        for (auto& e : glfw_events)
            XPutBackEvent(x11_display, &e);

        glfwPollEvents();

    #elif _WIN32
        #error "Implement me for Windows!"
    #else
        #error "Unknown OS"
    #endif
}