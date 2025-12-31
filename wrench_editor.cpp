#include "wrench_editor.h"
#include <glm/gtc/type_ptr.hpp>

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

void WrenchEditor::EndUI(std::vector<std::shared_ptr<Entity>>& entities, const glm::mat4& view, const glm::mat4& proj) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Begin("Hierarchy");
    for (auto& ent : entities) {
        if (ImGui::Selectable(ent->Name.c_str(), m_SelectedEntity == ent)) m_SelectedEntity = ent;
    }
    ImGui::End();

    ImGui::Begin("Inspector");
    if (m_SelectedEntity) {
        ImGui::DragFloat3("Pos", glm::value_ptr(m_SelectedEntity->Translation), 0.1f);
        ImGui::DragFloat3("Rot", glm::value_ptr(m_SelectedEntity->Rotation), 0.1f);
        ImGui::DragFloat3("Scale", glm::value_ptr(m_SelectedEntity->Scale), 0.1f);
    }
    ImGui::End();

    ImGui::Begin("Viewport");
    ImVec2 size = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(uintptr_t)m_Tex, size, { 0,1 }, { 1,0 });

    if (m_SelectedEntity) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        glm::mat4 model = m_SelectedEntity->GetWorldTransform();
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(model));
        if (ImGuizmo::IsUsing()) {
            float t[3], r[3], s[3];
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), t, r, s);
            m_SelectedEntity->Translation = { t[0], t[1], t[2] };
            m_SelectedEntity->Rotation = { r[0], r[1], r[2] };
            m_SelectedEntity->Scale = { s[0], s[1], s[2] };
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}