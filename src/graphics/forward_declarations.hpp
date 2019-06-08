#pragma once

#include <memory>

namespace grx {
    class Window;
    class Camera;
    class CameraManipulator;

    using CameraPtr            = std::shared_ptr<Camera>;
    using CameraManipulatorPtr = std::shared_ptr<CameraManipulator>;
}