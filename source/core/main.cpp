#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../renderer/renderer.h"
#include "../renderer/model.h"
#include "camera.h"
#include "../editor/wrench_editor.h"
#include "../components/mesh_component.h"
#include "../components/script_component.h"
#include "project_manager.h"
#include "script_system.h"
#include "scene_manager.h"
#include <iostream>
#include "common.h"


int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Visual Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!ScriptSystem::Instance().Initialize()) {
        std::cerr << "Failed to initialize script system!" << std::endl;
        return -1;
    }

    ProjectManager& pm = ProjectManager::Instance();
    if (!pm.LoadProject("test_project.vep")) {
        std::cout << "Creating new project..." << std::endl;
        pm.CreateNewProject("test_project.vep", "TestProject");
    }

    std::cout << "Project: " << pm.GetValue("project_name") << std::endl;
    std::cout << "Version: " << pm.GetValue("version") << std::endl;
    std::cout << "Assets Path: " << pm.GetValue("assets_path") << std::endl;

    WrenchEditor editor(window);
    Renderer renderer;
    Shader shader("default.vert", "default.frag");
    Model barrelModel("test_model.glb");

    std::vector<std::shared_ptr<Entity>> scene;

    auto barrel = std::make_shared<Entity>();
    barrel->Name = "Barrel";
    auto mc = barrel->addComponent<MeshComponent>();
    mc->meshModel = &barrelModel;

    auto scriptComp = barrel->addComponent<ScriptComponent>();
    scriptComp->LoadScript("example_script.as", "RotatingCube");

    scene.push_back(barrel);

    auto cube = std::make_shared<Entity>();
    cube->Name = "Oscillating Cube";
    auto cubeScript = cube->addComponent<ScriptComponent>();
    cubeScript->LoadScript("example_script.as", "Oscillator");
    auto cubeTransform = cube->getComponent<TransformComponent>();
    cubeTransform->translation = glm::vec3(3.0f, 0.0f, 0.0f);
    scene.push_back(cube);

    SceneManager::Instance().SetActiveScene(&scene);
    SceneManager::Instance().SetScenePath("scenes/main.scene");

    Camera camera;
    EditorState editorState = EditorState::EDITOR;

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        if (editorState == EditorState::PLAYING) {
            for (auto& entity : scene) {
                for (auto& component : entity->components) {
                    component->onUpdate(deltaTime);
                }
            }
        }

        editor.BeginUI();
        glm::mat4 proj = camera.GetProjectionMatrix(1280.0f, 720.0f);
        renderer.RenderScene(scene, shader, camera.GetViewMatrix(), proj);
        editor.EndUI(scene, camera, proj, editorState);

        EditorState newState = editor.GetEditorState();
        if (newState != editorState) {
            if (newState == EditorState::PLAYING) {
                std::cout << "Starting play mode..." << std::endl;
                SceneManager::Instance().SaveSceneState();

                for (auto& entity : scene) {
                    for (auto& component : entity->components) {
                        component->onInit();
                    }
                }
            }
            else {
                std::cout << "Stopping play mode..." << std::endl;
                SceneManager::Instance().RestoreSceneState();
            }
            editorState = newState;
        }

        glfwSwapBuffers(window);
    }

    ScriptSystem::Instance().Shutdown();
    glfwTerminate();
    return 0;
}