#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model;

class Entity {
public:
    std::string Name = "New Entity";
    glm::vec3 Translation = { 0,0,0 };
    glm::vec3 Rotation = { 0,0,0 };
    glm::vec3 Scale = { 1,1,1 };

    Model* meshModel = nullptr;
    Entity* Parent = nullptr;
    std::vector<std::shared_ptr<Entity>> Children;

    glm::mat4 GetLocalTransform() {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), Translation);
        m = glm::rotate(m, glm::radians(Rotation.x), { 1,0,0 });
        m = glm::rotate(m, glm::radians(Rotation.y), { 0,1,0 });
        m = glm::rotate(m, glm::radians(Rotation.z), { 0,0,1 });
        return glm::scale(m, Scale);
    }

    glm::mat4 GetWorldTransform() {
        if (Parent) return Parent->GetWorldTransform() * GetLocalTransform();
        return GetLocalTransform();
    }
};