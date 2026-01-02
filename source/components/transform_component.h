#pragma once
#include "component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformComponent : public Component {
public:
    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    glm::mat4 getLocalTransform() {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), translation);
        m = glm::rotate(m, glm::radians(rotation.x), { 1, 0, 0 });
        m = glm::rotate(m, glm::radians(rotation.y), { 0, 1, 0 });
        m = glm::rotate(m, glm::radians(rotation.z), { 0, 0, 1 });
        return glm::scale(m, scale);
    }
};