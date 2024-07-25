#ifndef _MODEL_BASE_HPP_
#define _MODEL_BASE_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

#include "Render/Material.hpp"
#include "Render/Vertex.hpp"
#include "Render/VertexArrayObject.hpp"

BEGIN_NAMESPACE(GLBase)

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
    glm::vec3 tangent;
};

struct ModelBase : VertexArray
{
    std::vector<Vertex> vertices;
    std::vector<int32_t> indices;
    std::shared_ptr<VertexArrayObject> vao = nullptr;
    std::shared_ptr<Material> material = nullptr;

    void InitVertexArray()
    {
        vertexSize = sizeof(Vertex);

        attributes.resize(4);
        attributes[0] = {3, sizeof(Vertex), offsetof(Vertex, position)};
        attributes[1] = {2, sizeof(Vertex), offsetof(Vertex, texCoords)};
        attributes[2] = {3, sizeof(Vertex), offsetof(Vertex, normal)};
        attributes[3] = {3, sizeof(Vertex), offsetof(Vertex, tangent)};

        vertexBuffer = vertices.empty() ? nullptr : (uint8_t *)&vertices[0];
        vertexBufferLength = vertices.size() * sizeof(Vertex);

        indexBuffer = indices.empty() ? nullptr : &indices[0];
        indexBufferLength = indices.size() * sizeof(int32_t);
    }
};

struct ModelMesh : ModelBase
{
    glm::mat4 transform = glm::mat4(1.0f);
};

END_NAMESPACE(GLBase)

#endif // _MODEL_BASE_HPP_