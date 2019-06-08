#include "CameraManipulator.hpp"

#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/constants.hpp>

#include "Window.hpp"
#include "Camera.hpp"

namespace InpEvt {
    enum {
        RotateX,
        RotateY,
        MoveFront,
        MoveBack,
        MoveUp,
        MoveDown,
        MoveRight,
        MoveLeft,
        RollLeft,
        RollRight,
        Accel,
        FovIn,
        FovOut
    };
}

grx::CameraManipulator::CameraManipulator(): _lastUpdateTime(glfwGetTime()) {}

void grx::CameraManipulator::updateStart() {
    auto currentTime = glfwGetTime();
    _timestep        = static_cast<float>(currentTime - _lastUpdateTime);
    _lastUpdateTime  = currentTime;
}



grx::CameraManipulatorFly::CameraManipulatorFly(float speed, float mouseSpeed):
        _speed(speed), _mouseSpeed(mouseSpeed)
{
    auto mouse_id    = input::ctx().mouse_id();
    auto keyboard_id = input::ctx().keyboard_id();

    input_map.MapFloat (InpEvt::RotateX,   mouse_id,    gainput::MouseAxisX          );
    input_map.MapFloat (InpEvt::RotateY,   mouse_id,    gainput::MouseAxisY          );
    input_map.MapBool  (InpEvt::FovIn,     mouse_id,    gainput::MouseButtonWheelUp  );
    input_map.MapBool  (InpEvt::FovOut,    mouse_id,    gainput::MouseButtonWheelDown);
    input_map.MapBool  (InpEvt::MoveFront, mouse_id,    gainput::MouseButtonLeft     );
    input_map.MapBool  (InpEvt::MoveBack,  mouse_id,    gainput::MouseButtonRight    );
    input_map.MapBool  (InpEvt::MoveUp,    keyboard_id, gainput::KeyW                );
    input_map.MapBool  (InpEvt::MoveDown,  keyboard_id, gainput::KeyS                );
    input_map.MapBool  (InpEvt::MoveRight, keyboard_id, gainput::KeyD                );
    input_map.MapBool  (InpEvt::MoveLeft,  keyboard_id, gainput::KeyA                );
    input_map.MapBool  (InpEvt::Accel,     keyboard_id, gainput::KeyShiftL           );
    input_map.MapBool  (InpEvt::RollLeft,  keyboard_id, gainput::KeyQ                );
    input_map.MapBool  (InpEvt::RollRight, keyboard_id, gainput::KeyE                );
}

void grx::CameraManipulatorFly::updateFov(grx::Window*, float& fov) {
    if (input_map.GetBool(InpEvt::FovIn))
        fov += 1.f;
    else if (input_map.GetBool(InpEvt::FovOut))
        fov -= 1.f;
}

void grx::CameraManipulatorFly::updateOrientation(Window* window, float& yaw, float& pitch, float& roll) {
    if (window)
        window->resetMousePos();

    auto x = input_map.GetFloat(InpEvt::RotateX);
    auto y = input_map.GetFloat(InpEvt::RotateY);

    // First frame bug :/
    if (x == 0.f) x = 0.5f;
    if (y == 0.f) y = 0.5f;

    yaw   -= _mouseSpeed * (0.5f - x);
    pitch -= _mouseSpeed * (0.5f - y);


    // Roll

    auto rollSp = _mouseSpeed * 0.006f;

    if (roll > 0.5f)
        roll = 0.5f;
    else if (roll < -0.5f)
        roll = -0.5f;

    if (input_map.GetBool(InpEvt::RollLeft))
        roll += rollSp;
    else if (input_map.GetBool(InpEvt::RollRight))
        roll -= rollSp;
    else if (roll != 0.f) {
        if (roll > 0.f) {
            roll -= rollSp;
            if (roll < 0.f)
                roll = 0.f;
        } else {
            roll += rollSp;
            if (roll > 0.f)
                roll = 0.f;
        }
    }
}

void grx::CameraManipulatorFly::updatePosition(Window*, vec3& position, vec3cr direction, vec3cr right, vec3cr up) {
    auto speed = input_map.GetBool(InpEvt::Accel) ? _speed * _shift_factor : _speed;

    if (input_map.GetBool(InpEvt::MoveFront))
        position += direction * _timestep * speed;

    if (input_map.GetBool(InpEvt::MoveBack))
        position -= direction * _timestep * speed;

    if (input_map.GetBool(InpEvt::MoveUp))
        position += up * _timestep * speed;

    if (input_map.GetBool(InpEvt::MoveDown))
        position -= up * _timestep * speed;

    if (input_map.GetBool(InpEvt::MoveRight))
        position += right * _timestep * speed;

    if (input_map.GetBool(InpEvt::MoveLeft))
        position -= right * _timestep * speed;
}