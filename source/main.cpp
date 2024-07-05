#include "Common/cpplang.hpp"

#include <glad/glad.h>
// GLFW (include after glad)
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Common/Logger.hpp"
#include "Model/AsModel.hpp"
#include "Model/Cube.hpp"
#include "Render/ShaderProgram.hpp"
#include "Viewer/Camera.hpp"

GLBase::AsModel *g_asModel = nullptr;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    if (!glfwInit())
    {
        LOGE("Failed to initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "GLBase", nullptr, nullptr);
    if (nullptr == window)
    {
        LOGE("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGE("Failed to initialize GLAD.");
        glfwTerminate();
        return -1;
    }

    g_asModel = new GLBase::AsModel("../assets/DamagedHelmet/DamagedHelmet.gltf");
    GLBase::ShaderProgram program;
    if (!program.loadFile("../source/Shader/GLSL/BasicPhong.vert", "../source/Shader/GLSL/BasicPhong.frag"))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }

    GLBase::Cube *lightCube = new GLBase::Cube(glm::vec3(-0.8f, 0.5f, 1.f), glm::vec3(0.1f, 0.1f, 0.1f));
    GLBase::ShaderProgram programLightCube;
    if (!programLightCube.loadFile("../source/Shader/GLSL/MiniGLSL.vert", "../source/Shader/GLSL/MiniGLSL.frag"))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }

    auto camera = std::make_shared<GLBase::Camera>();
    camera->lookat(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    camera->setPerspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 mvp = camera->getPerspectiveMatrix() * camera->getViewMatrix() * glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        program.setMat4("u_mvp", mvp);
        program.setVec3("lightColor", 0.5f, 1.f, 1.f);
        g_asModel->draw(program);
        lightCube->draw(programLightCube);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}