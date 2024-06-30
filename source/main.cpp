#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Common/Logger.hpp"
#include "Render/ShaderProgram.hpp"

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

const char *VS = R"(
    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
    )";


const char* FS = R"(

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
    )";

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
    if (!program.loadSource(VS, FS))
    {
        LOGE("Failed to initialize shader");
        glfwTerminate();
        return -1;
    }

    float vertices[] =
    {
        // positions | texture coords
	    0.8f, 0.9f, 0.0f, 1.0f, 1.0f,  // top right
	    0.8f, -0.7f, 0.0f, 1.0f, 0.0f, // bottom right
	    -0.8f, 0.9f, 0.0f, 0.0f, 1.0f, // top left

	    -0.8f, 0.8f, 0.0f, 0.0f, 1.0f, // top left
	    0.8f, -0.8f, 0.0f, 0.0f, 0.0f, // bottom right
	    -0.8f, -0.8f, 0.0f, 0.0f, 0.0f // bottom left
    };
    unsigned int indices[] =
    {
        0, 1, 2,    // first triangle
        3, 4, 5     // second triangle
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(GL_NONE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        program.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}