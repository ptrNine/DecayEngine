#include "Camera.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Window.hpp"
#include "CameraManipulator.hpp"

void grx::Camera::initDefaultCameraManipulator() {
    _cameraManipulator = std::make_shared<CameraManipulatorFly>();
}

grx::Camera& grx::Camera::look_at(const glm::vec3& pos) {
    if (pos != _pos) {
        _dir   =  glm::normalize(pos - _pos);
        _pitch =  (asinf(-_dir.y));
        _yaw   = -(atan2f(-_dir.x, -_dir.z));
        _roll  = 0.0f;

        modYaw();
        normalizePitch();

        _orient = glm::rotate(glm::mat4(1.f), _pitch, glm::vec3(1.f, 0.f, 0.f));
        _orient = glm::rotate(_orient, _yaw,   glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 inv_orient = glm::inverse(_orient);

        _right  = inv_orient * glm::vec4(1.f, 0.f,  0.f, 1.f);
        _up     = glm::cross (_right, _dir);
        _orient = glm::rotate(_orient, _roll, _dir);
    }

    return *this;
}

void log_mat4(const glm::mat4& m) {
    auto strs = std::array<std::array<std::string, 4>, 4>();
    auto max  = std::array<std::size_t, 4>{0};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            strs[i][j] = std::to_string(m[i][j]);
            if (max[j] < strs[i][j].length())
                max[j] = strs[i][j].length();
        }
    }

    for (auto& i : strs) {
        for (int j = 0; j < 4; ++j) {
            if (i[j][0] != '-')
                std::cout << ' ';

            std::cout << i[j] << " ";

            if (i[j][0] == '-') std::cout << " ";

            for (int k = 0; k < max[j] - i[j].length(); ++k)
                std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
grx::Camera::VP_T grx::Camera::update_view_projection(grx::Window *window) {
    if (_cameraManipulator && window && window->onFocus()) {
        _cameraManipulator->updateStart();
        _cameraManipulator->updateFov(window, _fov);
        _cameraManipulator->updateOrientation(window, _yaw, _pitch, _roll);

        modYaw();
        normalizeFov();
        normalizePitch();
        normalizeRoll();
    }
    //std::cout << "yaw: " << _yaw << ", pitch: " << _pitch << ", roll: " << _roll << std::endl;

    _orient = glm::rotate(glm::mat4(1.f), _pitch, glm::vec3(1.f, 0.f, 0.f));
    _orient = glm::rotate(_orient,          _yaw, glm::vec3(0.f, 1.f, 0.f));

    glm::mat4 inv_orient = glm::inverse(_orient);

    _dir    = inv_orient * glm::vec4(0.f, 0.f, -1.f, 1.f);
    _right  = inv_orient * glm::vec4(1.f, 0.f,  0.f, 1.f);

    _up     = glm::cross (_right,  _dir);
    _orient = glm::rotate(_orient, _roll, _dir);

    if (_cameraManipulator && window && window->onFocus())
        _cameraManipulator->updatePosition(window, _pos, _dir, _right, _up);

    _view        = _orient * glm::translate(glm::mat4(1.f), -_pos);
    _projection  = glm::perspective(glm::radians(_fov), _aspect, _zNear, _zFar);

    return VP_T(_view, _projection);
}

auto grx::Camera::extract_frustum_planes() -> FrustumT {
    auto vp = _projection * _view;

    FrustumT planes;
    planes[FrustumPlaneLeft  ].x = vp[0][3] + vp[0][0];
    planes[FrustumPlaneLeft  ].y = vp[1][3] + vp[1][0];
    planes[FrustumPlaneLeft  ].z = vp[2][3] + vp[2][0];
    planes[FrustumPlaneLeft  ].w = vp[3][3] + vp[3][0];

    planes[FrustumPlaneRight ].x = vp[0][3] - vp[0][0];
    planes[FrustumPlaneRight ].y = vp[1][3] - vp[1][0];
    planes[FrustumPlaneRight ].z = vp[2][3] - vp[2][0];
    planes[FrustumPlaneRight ].w = vp[3][3] - vp[3][0];

    planes[FrustumPlaneBottom].x = vp[0][3] + vp[0][1];
    planes[FrustumPlaneBottom].y = vp[1][3] + vp[1][1];
    planes[FrustumPlaneBottom].z = vp[2][3] + vp[2][1];
    planes[FrustumPlaneBottom].w = vp[3][3] + vp[3][1];

    planes[FrustumPlaneTop   ].x = vp[0][3] - vp[0][1];
    planes[FrustumPlaneTop   ].y = vp[1][3] - vp[1][1];
    planes[FrustumPlaneTop   ].z = vp[2][3] - vp[2][1];
    planes[FrustumPlaneTop   ].w = vp[3][3] - vp[3][1];

    planes[FrustumPlaneNear  ].x = vp[0][3] + vp[0][2];
    planes[FrustumPlaneNear  ].y = vp[1][3] + vp[1][2];
    planes[FrustumPlaneNear  ].z = vp[2][3] + vp[2][2];
    planes[FrustumPlaneNear  ].w = vp[3][3] + vp[3][2];

    planes[FrustumPlaneFar   ].x = vp[0][3] - vp[0][2];
    planes[FrustumPlaneFar   ].y = vp[1][3] - vp[1][2];
    planes[FrustumPlaneFar   ].z = vp[2][3] - vp[2][2];
    planes[FrustumPlaneFar   ].w = vp[3][3] - vp[3][2];

    return planes;
}

void grx::Camera::setCameraManipulator(CameraManipulator* cameraManipulator) {
    _cameraManipulator = CameraManipulatorPtr(cameraManipulator);
}

void grx::Camera::setCameraManipulator(const CameraManipulatorPtr& cameraManipulator) {
    _cameraManipulator = cameraManipulator;
}

grx::CameraManipulator* grx::Camera::getCameraManipulator() {
    return _cameraManipulator.get();
}