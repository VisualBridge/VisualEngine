#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "wrench_editor.h"
#include "renderer.h"
#include "model.h"

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Visual Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    WrenchEditor editor(window);
    Renderer renderer;
    Shader shader("default.vert", "default.frag");
    Model crateModel("test_model.glb");

    std::vector<std::shared_ptr<Entity>> scene;

    auto crate = std::make_shared<Entity>();
    crate->Name = "Crate";
    crate->meshModel = &crateModel;
    scene.push_back(crate);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), { 0, 0, -10 });

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        editor.BeginUI();

        renderer.RenderScene(scene, shader, view, proj);

        editor.EndUI(scene, view, proj);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}