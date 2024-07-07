#include "Common/cpplang.hpp"

#include <glad/glad.h>
// GLFW (include after glad)
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/Logger.hpp"
#include "Model/AsModel.hpp"
#include "Model/Cube.hpp"
#include "Render/ShaderProgram.hpp"
#include "Viewer/Camera.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

GLBase::AsModel *g_asModel = nullptr;

std::shared_ptr<GLBase::Camera> g_camera = nullptr;
glm::vec3 g_cameraPos = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 g_cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 g_cameraUp = glm::vec3(0.f, 1.f, 0.f);
float g_lastX;
float g_lastY;
bool g_firstMouse;

float g_deltaTime = 0.f;
float g_lastTime = 0.f;

void processInput(GLFWwindow* window)
{
    float currentTime = glfwGetTime();
    g_deltaTime = currentTime - g_lastTime;
    g_lastTime = currentTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_camera->movement(GLBase::FORWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_camera->movement(GLBase::BACKWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_camera->movement(GLBase::LEFT, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_camera->movement(GLBase::RIGHT, g_deltaTime);
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
    {
        g_firstMouse = true;
        return;
    }

    if (g_firstMouse)
    {
        g_firstMouse = false;
        g_lastX = xpos;
        g_lastY = ypos;
    }

    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos;

    g_lastX = xpos;
    g_lastY = ypos;

    g_camera->lookAround(xoffset, yoffset);
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

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GLBase", nullptr, nullptr);
    if (nullptr == window)
    {
        LOGE("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGE("Failed to initialize GLAD.");
        glfwTerminate();
        return -1;
    }

    g_asModel = new GLBase::AsModel("../assets/DamagedHelmet/DamagedHelmet.gltf");
    GLBase::ShaderProgram program;
    if (!program.loadFile("../source/Shader/GLSL/PhongGLSL.vert", "../source/Shader/GLSL/PhongGLSL.frag"))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }
    glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

    glm::vec3 lightPos = glm::vec3(-0.8f, 0.5f, 1.f);
    GLBase::Cube *lightCube = new GLBase::Cube(lightPos, glm::vec3(0.1f, 0.1f, 0.1f));
    GLBase::ShaderProgram programLightCube;
    if (!programLightCube.loadFile("../source/Shader/GLSL/MiniGLSL.vert", "../source/Shader/GLSL/MiniGLSL.frag"))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }

    g_camera = std::make_shared<GLBase::Camera>(g_cameraPos, g_cameraPos + g_cameraFront, g_cameraUp);
    g_camera->setPerspective(glm::radians(60.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = g_camera->getPerspectiveMatrix() * g_camera->getViewMatrix() * modelMatrix;

        program.use();
        program.setMat4("u_mvp", mvp);
        program.setMat4("u_model", modelMatrix);
        program.setMat3("u_inversTransModel", glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
        program.setVec3("lightColor", 1.f, 1.f, 1.f);
        program.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        program.setVec3("viewPos", g_camera->position().x, g_camera->position().y, g_camera->position().z);
        program.setVec3("material.ambient", 0.1f, 0.1f, 0.1f);
        program.setFloat("material.shininess", 64.f);
        g_asModel->draw(program);

        mvp = g_camera->getPerspectiveMatrix() * g_camera->getViewMatrix() * lightCube->getModelMatrix();
        lightCube->draw(programLightCube, mvp);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}