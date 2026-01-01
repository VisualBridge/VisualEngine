#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "Entity.h"
#include "Camera.h"

class WrenchEditor {
public:
    WrenchEditor(GLFWwindow* window);
    void BeginUI();
    void EndUI(
        std::vector<std::shared_ptr<Entity>>& entities,
        Camera& camera,
        const glm::mat4& proj
    );
    unsigned int GetFBO() const { return m_FBO; }

private:
    void CreateFramebuffer();
    unsigned int m_FBO, m_Tex, m_RBO;
    GLFWwindow* m_Window;
    std::shared_ptr<Entity> m_SelectedEntity = nullptr;
};
