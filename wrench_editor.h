#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include "Entity.h"

class WrenchEditor {
public:
    WrenchEditor(GLFWwindow* window);
    void BeginUI();
    void EndUI(std::vector<std::shared_ptr<Entity>>& entities, const glm::mat4& view, const glm::mat4& proj);
    unsigned int GetFBO() { return m_FBO; }

private:
    void CreateFramebuffer();
    unsigned int m_FBO, m_Tex, m_RBO;
    GLFWwindow* m_Window;
    std::shared_ptr<Entity> m_SelectedEntity = nullptr;
};