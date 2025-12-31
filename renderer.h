#pragma once
#include <glad/glad.h>
#include "Entity.h"
#include "Shader.h"
#include "Model.h"

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
        if (ent->meshModel) {
            shader.setMat4("model", ent->GetWorldTransform());
            ent->meshModel->Draw();
        }
        for (auto& child : ent->Children) {
            DrawEntity(child, shader);
        }
    }
};