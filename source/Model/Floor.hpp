#ifndef _FLOOR_HPP_
#define _FLOOR_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Render/ShaderProgram.hpp"

BEGIN_NAMESPACE(GLBase)

class Floor
{
public:
    Floor(glm::vec3 translate = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        m_model = glm::translate(glm::mat4(1.f), translate);
        m_model = glm::scale(m_model, scale);
    }

    ~Floor()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

public:
    void draw(ShaderProgram &shader, glm::mat4 &mvp)
    {
        shader.use();
        glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    glm::mat4 getModelMatrix() const
    {
        return m_model;
    }

private:
    static float vertices[12];
    static int indices[6];

    unsigned int VAO, VBO, EBO;
    glm::mat4 m_model = glm::mat4(1.0f);
};

float Floor::vertices[12] =
{
    -2.0, 0.01, 2.0,
    -2.0, 0.01, -2.0,
    2.0, 0.01, -2.0,
    2.0, 0.01, 2.0
};

int Floor::indices[6] =
{
    0, 2, 1,
    0, 3, 2
};

END_NAMESPACE(GLBase)

#endif // _FLOOR_HPP_
