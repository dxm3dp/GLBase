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
#include "Render/ShaderProgram.hpp"
#include "Viewer/Camera.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

GLBase::AsModel *g_asModel = nullptr;

std::shared_ptr<GLBase::Camera> g_camera = nullptr;
glm::vec3 g_cameraPos = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 g_cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 g_cameraUp = glm::vec3(0.f, 1.f, 0.f);

glm::vec3 g_lightPos = glm::vec3(-0.8f, 0.5f, 1.f);
glm::vec3 g_pointLightPositions[] =
{
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( -1.0f, 0.3f, -4.0f)
};

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

void setBlinnPhongShaderProgram(GLBase::ShaderProgram &program, glm::mat4 &modelMatrix);
void drawAsModel(GLBase::ShaderProgram &program);
void drawCube(GLBase::Cube *cube, GLBase::ShaderProgram &program);
void drawFloor(GLBase::Floor *floor, GLBase::ShaderProgram &program);
void loadShader(GLBase::ShaderProgram &program, const std::string &vsPath, const std::string &fsPath);

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

    glm::mat4 modelMatrix1 = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 modelMatrix2 = glm::translate(modelMatrix1, glm::vec3(-3.f, -4.f, -1.f));
    modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(-45.f), glm::vec3(0.f, 0.f, 1.f));

    GLBase::Cube *lightCube1 = new GLBase::Cube(g_pointLightPositions[0], glm::vec3(0.05f, 0.05f, 0.05f));
    GLBase::Cube *lightCube2 = new GLBase::Cube(g_pointLightPositions[1], glm::vec3(0.05f, 0.05f, 0.05f));
    GLBase::Floor *floor = new GLBase::Floor(glm::vec3(0.f, -1.f, 0.f), glm::vec3(2.f, 1.f, 2.f));

    GLBase::ShaderProgram program;
    loadShader(program, "../source/Shader/GLSL/BlinnPhongMultiLights.vert", "../source/Shader/GLSL/BlinnPhongMultiLights.frag");

    GLBase::ShaderProgram programLightCube;
    loadShader(programLightCube, "../source/Shader/GLSL/MiniGLSL.vert", "../source/Shader/GLSL/MiniGLSL.frag");

    GLBase::ShaderProgram programShadowMapping;
    loadShader(programShadowMapping, "../source/Shader/GLSL/BlinnPhongShadowMapping.vert", "../source/Shader/GLSL/BlinnPhongShadowMapping.frag");

    g_camera = std::make_shared<GLBase::Camera>(g_cameraPos, g_cameraPos + g_cameraFront, g_cameraUp);
    g_camera->setPerspective(glm::radians(60.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setBlinnPhongShaderProgram(program, modelMatrix1);
        drawAsModel(program);

        setBlinnPhongShaderProgram(program, modelMatrix2);
        drawAsModel(program);

        drawCube(lightCube1, programLightCube);
        drawCube(lightCube2, programLightCube);
        drawFloor(floor, programLightCube);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void setBlinnPhongShaderProgram(GLBase::ShaderProgram &program, glm::mat4 &modelMatrix)
{
    program.use();
    // set model view projection matrix
    program.setMat4("u_modelMat", modelMatrix);
    program.setMat4("u_viewMat", g_camera->getViewMatrix());
    program.setMat4("u_projectionMat", g_camera->getPerspectiveMatrix());
    // set view position
    program.setVec3("u_viewPos", g_camera->position().x, g_camera->position().y, g_camera->position().z);
    // set directional light
    program.setVec3("u_dirLight.direction", -1.f, -1.f, -1.f);
    program.setVec3("u_dirLight.ambient", 0.05f, 0.05f, 0.05f);
    program.setVec3("u_dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    program.setVec3("u_dirLight.specular", 0.5f, 0.5f, 0.5f);
    program.setVec3("u_material.ambient", 0.1f, 0.1f, 0.1f);
    program.setFloat("u_material.shininess", 64.f);
    // set point light 1
    program.setVec3("u_pointLight[0].position", g_pointLightPositions[0].x, g_pointLightPositions[0].y, g_pointLightPositions[0].z);
    program.setVec3("u_pointLight[0].ambient", 0.05f, 0.05f, 0.05f);
    program.setVec3("u_pointLight[0].diffuse", 0.8f, 0.8f, 0.8f);
    program.setVec3("u_pointLight[0].specular", 1.0f, 1.0f, 1.0f);
    program.setFloat("u_pointLight[0].constant", 1.0f);
    program.setFloat("u_pointLight[0].linear", 0.09f);
    program.setFloat("u_pointLight[0].quadratic", 0.032f);
    // set point light 2
    program.setVec3("u_pointLight[1].position", g_pointLightPositions[1].x, g_pointLightPositions[1].y, g_pointLightPositions[1].z);
    program.setVec3("u_pointLight[1].ambient", 0.05f, 0.05f, 0.05f);
    program.setVec3("u_pointLight[1].diffuse", 0.8f, 0.8f, 0.8f);
    program.setVec3("u_pointLight[1].specular", 1.0f, 1.0f, 1.0f);
    program.setFloat("u_pointLight[1].constant", 1.0f);
    program.setFloat("u_pointLight[1].linear", 0.09f);
    program.setFloat("u_pointLight[1].quadratic", 0.032f);
}

void drawAsModel(GLBase::ShaderProgram &program)
{
    g_asModel->draw(program);
}

void drawCube(GLBase::Cube *cube, GLBase::ShaderProgram &program)
{
    glm::mat4 mvp = g_camera->getPerspectiveMatrix() * g_camera->getViewMatrix() * cube->getModelMatrix();

    cube->draw(program, mvp);
}

void drawFloor(GLBase::Floor *floor, GLBase::ShaderProgram &program)
{
    glm::mat4 mvp = g_camera->getPerspectiveMatrix() * g_camera->getViewMatrix() * floor->getModelMatrix();

    floor->draw(program, mvp);
}

void loadShader(GLBase::ShaderProgram &program, const std::string &vsPath, const std::string &fsPath)
{
    if (!program.loadFile(vsPath, fsPath))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
    }
}