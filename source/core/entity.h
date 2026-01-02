#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../components/component.h"
#include "../components/transform_component.h"

class Entity {
public:
    std::string Name = "New Entity";
    Entity* Parent = nullptr;
    std::vector<std::shared_ptr<Entity>> Children;
    std::vector<std::shared_ptr<Component>> components;

    Entity() {
        addComponent<TransformComponent>();
    }

    template<typename T>
    std::shared_ptr<T> addComponent() {
        auto comp = std::make_shared<T>();
        comp->owner = this;
        components.push_back(comp);
        comp->onInit();
        return comp;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() {
        for (auto& comp : components) {
            auto casted = std::dynamic_pointer_cast<T>(comp);
            if (casted) return casted;
        }
        return nullptr;
    }

    glm::mat4 getWorldTransform() {
        auto transform = getComponent<TransformComponent>();
        glm::mat4 local = transform ? transform->getLocalTransform() : glm::mat4(1.0f);
        if (Parent) return Parent->getWorldTransform() * local;
        return local;
    }
};