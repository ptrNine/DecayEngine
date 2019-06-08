#pragma once

#include <gainput/gainput.h>

#include <defines.hpp>

#ifdef __linux__
    #include <vector>
    typedef struct _XDisplay Display;
#elif _WIN32
    #error "Implement me for Windows!"
#else
    #error "Unknown OS"
#endif

namespace input_impl {
    class InputContext {
    public:
        void setDisplaySize(int w, int h) {
            manager.SetDisplaySize(w, h);
        }

        auto createInputMap() {
            return gainput::InputMap(manager);
        }

        inline const gainput::DeviceId mouse_id()    { return _mouse_id; }
        inline const gainput::DeviceId keyboard_id() { return _keyboard_id; }
        inline const gainput::DeviceId gamepad_id()  { return _gamepad_id; }

        void update();

    protected:
        #ifdef __linux__
            Display* x11_display;
            std::vector<XEvent> glfw_events;
        #elif _WIN32
            #error "Implement me for Windows!"
        #else
            #error "Unknown OS"
        #endif

        gainput::InputManager manager;
        const gainput::DeviceId _mouse_id    = manager.CreateDevice<gainput::InputDeviceMouse>();
        const gainput::DeviceId _keyboard_id = manager.CreateDevice<gainput::InputDeviceKeyboard>();
        const gainput::DeviceId _gamepad_id  = manager.CreateDevice<gainput::InputDevicePad>();

        DE_MARK_AS_SINGLETON(InputContext);
    };
} // namespace input

namespace input {
    /**
     * @return Input context
     */
    inline auto& ctx() {
        return input_impl::InputContext::instance();
    }
}