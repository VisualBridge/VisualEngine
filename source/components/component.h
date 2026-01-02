#pragma once

class Entity;

class Component {
public:
    Entity* owner = nullptr;
    virtual ~Component() {}
    virtual void onInit() {}
    virtual void onUpdate(float deltaTime) {}
};