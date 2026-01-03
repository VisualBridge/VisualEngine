#include "wrench_editor.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include "../components/transform_component.h"
#include "../components/mesh_component.h"
#include "../components/script_component.h"

WrenchEditor::~WrenchEditor() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_Tex);
    glDeleteRenderbuffers(1, &m_RBO);
}

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

void WrenchEditor::DrawToolbar(EditorState& currentState) {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

    if (m_EditorState == EditorState::EDITOR) {
        if (ImGui::Button("▶ Play", ImVec2(100, 40))) {
            m_EditorState = EditorState::PLAYING;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("Editor Mode");
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button("■ Stop", ImVec2(100, 40))) {
            m_EditorState = EditorState::EDITOR;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "▶ PLAYING");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::End();
}

void WrenchEditor::EndUI(
    std::vector<std::shared_ptr<Entity>>& entities,
    Camera& camera,
    const glm::mat4& proj,
    EditorState& currentState
) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    DrawToolbar(currentState);

    ImGui::Begin("Hierarchy");
    for (auto& ent : entities) {
        if (ImGui::Selectable(ent->Name.c_str(), m_SelectedEntity == ent)) {
            m_SelectedEntity = ent;
        }
    }
    ImGui::End();

    ImGui::Begin("Inspector");
    if (m_SelectedEntity) {
        ImGui::Text("Entity: %s", m_SelectedEntity->Name.c_str());
        ImGui::Separator();

        auto transform = m_SelectedEntity->getComponent<TransformComponent>();
        if (transform) {
            ImGui::Text("Transform");
            ImGui::Separator();

            if (m_EditorState == EditorState::EDITOR) {
                ImGui::DragFloat3("Position", glm::value_ptr(transform->translation), 0.1f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform->rotation), 0.1f);
                ImGui::DragFloat3("Scale", glm::value_ptr(transform->scale), 0.1f);
            }
            else {
                ImGui::Text("Position: %.2f, %.2f, %.2f", transform->translation.x, transform->translation.y, transform->translation.z);
                ImGui::Text("Rotation: %.2f, %.2f, %.2f", transform->rotation.x, transform->rotation.y, transform->rotation.z);
                ImGui::Text("Scale: %.2f, %.2f, %.2f", transform->scale.x, transform->scale.y, transform->scale.z);
            }
        }

        auto mesh = m_SelectedEntity->getComponent<MeshComponent>();
        if (mesh) {
            ImGui::Spacing();
            ImGui::Text("Mesh Component");
            ImGui::Separator();
            ImGui::Text("Model: %s", mesh->meshModel ? "Loaded" : "Empty");
        }

        auto script = m_SelectedEntity->getComponent<ScriptComponent>();
        if (script) {
            ImGui::Spacing();
            ImGui::Text("Script Component");
            ImGui::Separator();
            ImGui::Text("Script: %s", script->scriptPath.c_str());
            ImGui::Text("Class: %s", script->className.c_str());
            ImGui::Text("Status: %s", script->scriptObject ? "Active" : "Inactive");
        }
    }
    ImGui::End();

    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(uintptr_t)m_Tex, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsWindowHovered() && m_EditorState == EditorState::EDITOR) {
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

    if (m_SelectedEntity && m_EditorState == EditorState::EDITOR) {
        auto transform = m_SelectedEntity->getComponent<TransformComponent>();
        if (transform) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 modelMatrix = m_SelectedEntity->getWorldTransform();

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(proj),
                ImGuizmo::TRANSLATE,
                ImGuizmo::LOCAL,
                glm::value_ptr(modelMatrix)
            );

            if (ImGuizmo::IsUsing()) {
                float t[3], r[3], s[3];
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), t, r, s);
                transform->translation = { t[0], t[1], t[2] };
                transform->rotation = { r[0], r[1], r[2] };
                transform->scale = { s[0], s[1], s[2] };
            }
        }
    }

    if (m_EditorState == EditorState::PLAYING) {
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "▶ PLAYING");
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