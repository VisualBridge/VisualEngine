#pragma once
#include <glad/glad.h>
#include "../core/entity.h"
#include "../components/mesh_component.h"
#include "shader.h"

class Renderer {
public:
    void RenderScene(const std::vector<std::shared_ptr<Entity>>& entities, Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);

        for (auto& ent : entities) {
            DrawEntity(ent, shader);
        }
    }

private:
    void DrawEntity(std::shared_ptr<Entity> ent, Shader& shader) {
        auto mesh = ent->getComponent<MeshComponent>();
        if (mesh && mesh->meshModel) {
            shader.setMat4("model", ent->getWorldTransform());
            mesh->meshModel->Draw(shader);
        }
        for (auto& child : ent->Children) {
            DrawEntity(child, shader);
        }
    }
};