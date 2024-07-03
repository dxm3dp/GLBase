#include "Common/cpplang.hpp"

#include <glad/glad.h>
// GLFW (include after glad)
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/Logger.hpp"
#include "Model/AsModel.hpp"
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

    GLBase::ShaderProgram program;
    if (!program.loadFile("../source/Shader/GLSL/MiniGLSL.vert", "../source/Shader/GLSL/MiniGLSL.frag"))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }

    g_asModel = new GLBase::AsModel("../assets/diablo3/diablo3_pose.obj");

    auto camera = std::make_shared<GLBase::Camera>();
    camera->lookat(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    camera->setPerspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 mvp = camera->getPerspectiveMatrix() * camera->getViewMatrix();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        GLint location = glGetUniformLocation(program.getId(), "u_mvp");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));

        g_asModel->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}