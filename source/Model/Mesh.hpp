#ifndef _MESH_HPP_
#define _MESH_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

BEGIN_NAMESPACE(GLBase)

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)// TODO 引用类型？
    {
        m_vertices = vertices;
        m_indices = indices;

        setupMesh();
    }

public:
    void draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    unsigned int VAO, VBO, EBO;
};

END_NAMESPACE(GLBase)

#endif // _MESH_HPP_