#ifndef _MESH_HPP_
#define _MESH_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Model/ModelBase.hpp"
#include "Render/ProgramGLSL.hpp"

BEGIN_NAMESPACE(GLBase)

struct Textures
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Textures> textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;

        setupMesh();
    }

public:
    void draw(ProgramGLSL &shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int normalNr = 1;
        unsigned int specularNr = 1;
        std::string name;
        for(int i = 0; i < m_textures.size(); i++)
        {
            if (m_textures[i].type == "texture_diffuse")
            {
                name = "texture_diffuse" + std::to_string(diffuseNr++);
            }
            else if (m_textures[i].type == "texture_normal")
            {
                name = "texture_normal" + std::to_string(normalNr++);
            }
            else if (m_textures[i].type == "texture_specular")
            {
                name = "texture_specular" + std::to_string(specularNr++);
            }
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
            glUniform1i(glGetUniformLocation(shader.getId(), name.c_str()), i);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

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
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Textures> m_textures;

    unsigned int VAO, VBO, EBO;
};

END_NAMESPACE(GLBase)

#endif // _MESH_HPP_