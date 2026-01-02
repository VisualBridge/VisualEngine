#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../renderer/renderer.h"
#include "../renderer/model.h"
#include "../core/camera.h"
#include "../editor/wrench_editor.h"
#include "../components/mesh_component.h"

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Visual Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    WrenchEditor editor(window);
    Renderer renderer;
    Shader shader("default.vert", "default.frag");
    Model barrelModel("test_model.glb");

    std::vector<std::shared_ptr<Entity>> scene;

    // Barrel Setup
    auto barrel = std::make_shared<Entity>();
    barrel->Name = "Barrel";
    auto mc = barrel->addComponent<MeshComponent>();
    mc->meshModel = &barrelModel;
    scene.push_back(barrel);

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