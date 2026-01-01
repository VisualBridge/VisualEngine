#include "wrench_editor.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include "Entity.h"
#include "Camera.h"

WrenchEditor::WrenchEditor(GLFWwindow* window) : m_Window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    CreateFramebuffer();
}

void WrenchEditor::CreateFramebuffer() {
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glGenTextures(1, &m_Tex);
    glBindTexture(GL_TEXTURE_2D, m_Tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0);
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WrenchEditor::BeginUI() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void WrenchEditor::EndUI(
    std::vector<std::shared_ptr<Entity>>& entities,
    Camera& camera,
    const glm::mat4& proj
) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Begin("Hierarchy");
    for (auto& ent : entities) {
        if (ImGui::Selectable(ent->Name.c_str(), m_SelectedEntity == ent)) {
            m_SelectedEntity = ent;
        }
    }
    ImGui::End();

    ImGui::Begin("Inspector");
    if (m_SelectedEntity) {
        ImGui::DragFloat3("Position", glm::value_ptr(m_SelectedEntity->Translation), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(m_SelectedEntity->Rotation), 0.1f);
        ImGui::DragFloat3("Scale", glm::value_ptr(m_SelectedEntity->Scale), 0.1f);
    }
    ImGui::End();

    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image(
        (ImTextureID)(uintptr_t)m_Tex,
        viewportSize,
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    if (ImGui::IsWindowHovered()) {
        if (ImGui::IsKeyDown(ImGuiKey_W)) camera.Move(0.0f, 0.0f, 0.1f);
        if (ImGui::IsKeyDown(ImGuiKey_S)) camera.Move(0.0f, 0.0f, -0.1f);
        if (ImGui::IsKeyDown(ImGuiKey_A)) camera.Move(-0.1f, 0.0f, 0.0f);
        if (ImGui::IsKeyDown(ImGuiKey_D)) camera.Move(0.1f, 0.0f, 0.0f);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            camera.Rotate(mouseDelta.x * 0.1f, -mouseDelta.y * 0.1f);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }
    }

    if (m_SelectedEntity) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(
            ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y,
            ImGui::GetWindowWidth(),
            ImGui::GetWindowHeight()
        );
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = m_SelectedEntity->GetWorldTransform();
        ImGuizmo::Manipulate(
            glm::value_ptr(view),
            glm::value_ptr(proj),
            ImGuizmo::TRANSLATE,
            ImGuizmo::LOCAL,
            glm::value_ptr(model)
        );
        if (ImGuizmo::IsUsing()) {
            float translation[3], rotation[3], scale[3];
            ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(model),
                translation,
                rotation,
                scale
            );
            m_SelectedEntity->Translation = { translation[0], translation[1], translation[2] };
            m_SelectedEntity->Rotation = { rotation[0], rotation[1], rotation[2] };
            m_SelectedEntity->Scale = { scale[0], scale[1], scale[2] };
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backupContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupContext);
    }
}
