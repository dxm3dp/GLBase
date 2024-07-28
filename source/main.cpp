#include "Common/cpplang.hpp"

#include <glad/glad.h>
// GLFW (include after glad)
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/Logger.hpp"
#include "Model/AsModel.hpp"
#include "Model/Cube.hpp"
#include "Model/Floor.hpp"
#include "Model/ModelLoader.hpp"
#include "Render/ShadowMapping.hpp"
#include "Render/ProgramGLSL.hpp"
#include "Render/Renderer.hpp"
#include "Viewer/Camera.hpp"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

GLBase::AsModel *g_asModel = nullptr;

glm::vec3 g_floorPos = glm::vec3(0.0f, -1.0f, 0.0f);

glm::vec3 g_lightPos = glm::vec3(1.0f, 4.0f, 1.0f);
glm::vec3 g_pointLightPositions[] =
{
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( -1.0f, 0.3f, -4.0f)
};

std::shared_ptr<GLBase::Camera> g_camera = nullptr;
glm::vec3 g_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 g_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

    g_camera = std::make_shared<GLBase::Camera>(g_cameraPos, g_cameraPos + g_cameraFront, g_cameraUp);
    g_camera->setPerspective(glm::radians(GLBase::CAMERA_FOV), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, GLBase::CAMERA_NEAR, GLBase::CAMERA_FAR);

    //GLBase::ShadowMapping shadowMapping;

    GLBase::ModelLoader modelLoader;
    modelLoader.loadFloor(modelLoader.getScene().floor);
    modelLoader.loadCube(modelLoader.getScene().cube, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    modelLoader.loadModel("../assets/DamagedHelmet/DamagedHelmet.gltf", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.1f, 0.0f)));

    GLBase::Renderer renderer;
    renderer.create(g_camera, modelLoader.getScene());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.setupScene();

        renderer.drawScene(false);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void loadShader(GLBase::ProgramGLSL &program, const std::string &vsPath, const std::string &fsPath)
{
    if (!program.loadFile(vsPath, fsPath))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
    }
}