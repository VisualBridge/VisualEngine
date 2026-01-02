#pragma once
#include "component.h"
#include "../core/camera.h"

class CameraComponent : public Component {
public:
    bool isMain = true;
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    void onUpdate(float delta) override {
    }
};