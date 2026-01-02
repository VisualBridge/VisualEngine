#pragma once
#include "component.h"
#include <string>

class asIScriptObject;

class ScriptComponent : public Component {
public:
    std::string scriptPath;
    asIScriptObject* scriptObject = nullptr;

    void onInit() override {
    }

    void onUpdate(float delta) override {
    }
};