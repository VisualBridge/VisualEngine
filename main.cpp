#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "wrench_editor.h"
#include "renderer.h"
#include "model.h"
#include "Camera.h"

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

    Camera camera;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        editor.BeginUI();

        glm::mat4 proj = camera.GetProjectionMatrix(1280.0f, 720.0f);
        renderer.RenderScene(scene, shader, camera.GetViewMatrix(), proj);
        editor.EndUI(scene, camera, proj);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
